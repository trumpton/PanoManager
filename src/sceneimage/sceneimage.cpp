//    PanoManager - Interactive panorama tour manager program
//    Copyright (C) 2018  Steve M Clarke
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

//
// Scene Image
//

#include "sceneimage.h"
#include <math.h>
#include <QDir>
#include <QRgb>
#include <QFile>
#include <QApplication>
#include <QStandardPaths>
#include "../errors/pmerrors.h"

SceneImage::SceneImage() : QObject()
{
    clear() ;
}

void SceneImage::clear()
{
    m_loadMax=0 ;
    m_loadPos=0 ;
    m_buildFace=0 ;
    m_filename = "" ;
    m_facedir = "" ;
    m_abort = false ;
    for (int i=0; i<6; i++) {
        m_faces[i].clear() ;
    }
}

// TODO: if preview exists, and ask for load hires if avail, nothing is loaded
// loadpreview=true, buildpreview=false, buildonly=false
PM::Err SceneImage::loadImage(QString imagefile, bool loadpreview, bool buildpreview, bool scaleforpreview, bool buildonly)
{

   emit(progressUpdate("Loading Faces ...")) ;

   PM::Err err = PM::Ok ;
   QFileInfo finfo(imagefile) ;
   clear() ;
   m_filename = imagefile ;
   m_facedir = finfo.canonicalPath() + "/" + finfo.baseName();

   bool dobuild = (!facesExist(m_filename) && !buildpreview)                        // Full Res
                || (!previewExists(m_filename) && (loadpreview || buildpreview)) ;  // Preview

   m_loadPos=0 ;
   if (dobuild) m_loadMax=100 ;
   if (!buildonly) m_loadMax+=100 ;

   if  (dobuild) {
       err = buildFaces(buildpreview) ;
       m_loadPos=100 ;
   }

   if (err==PM::Ok && !buildonly) {

       if (loadpreview) {

           // Load preview as requested
           if (previewExists(m_filename)) err = loadFaces(true, true) ;
           else err = PM::PreviewLoadError ;

       } else {

           // Load the faces
           if (facesExist(m_filename)) err = loadFaces(false, scaleforpreview) ;
           else err = PM::FaceLoadError ;

       }

   }

   return err ;

}



bool SceneImage::previewExists(QString imagefile)
{
    QFileInfo f(imagefile) ;
    bool previewexists = true ;

    for (int i=0; i<6; i++) {
        QString path = f.canonicalPath() + "/" + f.baseName() + "/face00" ;
        QFile f2(path + QString::number(i) + "_preview.png") ;
        if (!f2.exists() || f2.size()==0) previewexists=false ;
    }
    return previewexists ;
}


bool SceneImage::facesExist(QString imagefile)
{
    QFileInfo f(imagefile) ;

    bool facesexist = true ;

    for (int i=0; i<6; i++) {
        QString path = f.canonicalPath() + "/" + f.baseName() + "/face00" ;
        QFile f1(path + QString::number(i) + ".png") ;
        if (!f1.exists() || f1.size()==0) facesexist=false ;
    }
    return facesexist ;
}



// Load faces, and scale to 512x512
PM::Err SceneImage::loadFaces(bool loadpreview, bool scaleforpreview)
{
    if (m_facedir.isEmpty()) return PM::InputNotDefined ;

    PM::Err err = PM::Ok ;
    m_ispreview = loadpreview ;

    for (int f=0; err==PM::Ok && f<6; f++) {
        emit(progressUpdate(QString("Loading Face: ") + QString::number(f))) ;
        QString path = m_facedir + "/face00" + QString::number(f) + QString(loadpreview?"_preview.png":".png") ;
        if (m_faces[f].load(path)) {
            if (scaleforpreview) {
                emit(progressUpdate(QString("Scaling Face: ") + QString::number(f))) ;
                m_faces[f] = m_faces[f].scaled(512, 512) ;
            }
            int prog = m_loadPos+(100*f)/6 ;
            emit(percentUpdate( (prog*100)/m_loadMax )) ;
        } else {
            if (loadpreview) {
                err = PM::PreviewLoadError ;
            } else {
                err = PM::FaceLoadError ;
            }
        }
    }
    return err ;
}


Face &SceneImage::getFace(int n)
{
    return m_faces[n] ;
}


// Map a part of the equirectangular panorama (in) to a cube face, using the MapTranslation
// which provides the corresponding equirectangular coordinates for each face coordinate.
// buildFaces will move progress bar (prog) on by 1300.
PM::Err SceneImage::buildFaces(bool buildpreview) {

    QImage scaledsource ;

    // Actual equirectangular file width and height
    unsigned long int filewidth, fileheight ;
    int scaledfilewidth, scaledfileheight ;

    PM::Err err = PM::Ok ;

    {
        // Work with a smoothed and scaled version of the source for faster and more accurate colour smoothing
        QImage file ;

        emit(progressUpdate(QString("Loading Equirectangular Image")));

        if (!file.load(m_filename)) {

            err = PM::EquirectReadError ;

        } else {

            filewidth = file.width() ;
            fileheight = file.height() ;

            // Calculate the largest source scale (7 downto 1) because it
            // looks like the Linux QImage.scaled function can't handled > 32767
            // And Windows 32 bit applications have a 2Gb memory limitation
            // So limit scaled image to 16384x8192 => 512Mb
            int filescale=7 ;
            do {
                scaledfilewidth = filewidth * filescale ;
                scaledfileheight = fileheight * filescale ;
                filescale-- ;
            } while ((scaledfilewidth>8192 || scaledfileheight>4096) && filescale>1) ;

            emit(progressUpdate(QString("UpScaling Equirectangular Image")));

            scaledsource = file.scaled(scaledfilewidth, scaledfileheight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation) ;

        }
    }

    int workingsize ;   // Size the face is generated at (3 * equirectangular image height)
    int outputsize ;    // Size the face is output at (i.e. equirectangular image height x height)

    if (buildpreview) {
        // Previews can use smaller size, and the quality is less important - speed is of the essence
        workingsize = 512 ;
        outputsize = 512 ;
    } else {
        // Use prime number for working size multiplier as better smooting achieved
        workingsize = fileheight * 3 ;
        outputsize = fileheight ;
    }

    QDir dir ;

    if (err==PM::Ok && !dir.exists(m_facedir) && !dir.mkdir(m_facedir)) {
        err = PM::OutputWriteError ;
    }

    for (int f=0; err==PM::Ok && f<6; f++) {
        m_buildFace = f ;
        if (err==PM::Ok) {

            Face face ;
            emit(progressUpdate(QString("Building Face: ") + QString::number(f)));
            connect(&face, SIGNAL(percentUpdate(int)), this, SLOT(handlePercentUpdate(int))) ;
            connect(&face, SIGNAL(progressUpdate(QString)), this, SLOT(handleProgressUpdate(QString))) ;
            connect(this, SIGNAL(abort()), &face, SLOT(handleAbort())) ;
            err = face.build(scaledsource, f, workingsize) ;
            disconnect(this, SIGNAL(abort()), &face, SLOT(handleAbort())) ;
            disconnect(&face, SIGNAL(progressUpdate(QString)), this, SLOT(handleProgressUpdate(QString))) ;
            disconnect(&face, SIGNAL(percentUpdate(int)), this, SLOT(handlePercentUpdate(int))) ;

            if (err==PM::Ok) {
                emit(progressUpdate(QString("Saving Face: ") + QString::number(f)));
                face = face.scaled(outputsize, outputsize) ;
                face.save(m_facedir + "/face00" + QString::number(f) + QString(buildpreview?"_preview.png":".png")) ;
            }
        }
    }

    return err ;
}

void SceneImage::handleProgressUpdate(QString message)
{
    emit( progressUpdate(message)) ;
}


void SceneImage::handleAbort()
{
    m_abort = true ;
    emit(abort()) ;
}


// Handle % processing of face function
void SceneImage::handlePercentUpdate(int percent)
{
    int prog = (100*(m_loadPos+(100*m_buildFace+percent))/(6*m_loadMax)) ;
    emit(percentUpdate( prog )) ;
}
