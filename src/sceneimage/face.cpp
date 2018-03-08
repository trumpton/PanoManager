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
// Face
//

#include "face.h"
#include "maptranslation/maptranslation.h"

#include <QImage>
#include <QString>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>

Face::Face()
{
}


Face::Face(const QImage& img)
{
    m_image = img ;
}

Face::~Face()
{
}

////////////////////////////////////////////////////////////////////
/// \brief Face::clear
/// \return
///

void Face::clear()
{
    QImage tiny ;
    m_image = tiny ;
}

////////////////////////////////////////////////////////////////////
/// \brief Face::load
/// \param filename
/// \return
///

bool Face::load(QString filename)
{
    return m_image.load(filename) ;
}


////////////////////////////////////////////////////////////////////
/// \brief Face::image
/// \return
///

QImage& Face::image()
{
    return m_image ;
}

////////////////////////////////////////////////////////////////////
/// \brief Face::build
/// \param prog
/// \param source
/// \param f
/// \param size
/// \return
///

// Advances prog by 200
PM::Err Face::build(ProgressDialog *prog, QImage source, int f, int size)
{
    if (!prog) return PM::InvalidPointer ;
    if (source.isNull()) return PM::InputNotDefined ;

    // TODO: This is already loaded in sceneimage::buildfaces
    // TODO: return true/false
    MapTranslation map ;

    int srcx = source.width() ;
    int srcy = source.height() ;
    int dstxy = size ;

    PM::Err err = PM::Ok ;

    // Start map (advances prog by 100)
    err = map.start(prog, f, srcx, srcy, dstxy) ;
    if (err!=PM::Ok) return err ;

    // This function advances prog by 100
    int progstart = prog->value() ;

    m_image = QImage(dstxy, dstxy, QImage::Format_ARGB32) ;
    MapCoordinate *coord ;
    unsigned long int iteration=0 ;

    prog->setText2(QString("Building Face ") + QString::number(f)) ;

    do {
        coord=map.next() ;
        if (++iteration%501==1) {

            // TODO: Need a tidier exist than this
            if (prog->isCancelled()) err=PM::OperationCancelled ;

            prog->setValue(progstart + (100*iteration)/(dstxy*dstxy)) ;
        }
        if (coord->dstx > dstxy || coord->dsty>dstxy) err = PM::InvalidMapTranslation ;
        QRgb pix= source.pixel(coord->srcx, coord->srcy) ;
        m_image.setPixel(coord->dsty, coord->dstx, pix) ;

    } while (coord->face>=0 && iteration<=(dstxy*dstxy)) ;

    prog->setValue(progstart+100) ;

    if (coord->face>=0) err = PM::InvalidMapTranslation ;

    map.end() ;
    m_image = m_image.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation) ;

    return err ;
}


////////////////////////////////////////////////////////////////////
/// \brief Face::exportTiles
/// \param prog
/// \param targetimagesize
/// \param tilesize
/// \param outputFolder
/// \param mask
/// \return
///

PM::Err Face::exportTiles(ProgressDialog *prog, int targetimagesize, int tilesize, QString outputFolder, QString mask)
{
    if (!prog) return PM::InvalidPointer ;
    if (outputFolder.isEmpty()) return PM::OutputNotDefined ;
    if (targetimagesize<=0) return PM::InvalidTargetImageSize ;

    int progstart = prog->value() ;

    int width = targetimagesize ;
    int height = targetimagesize ;

    QImage img = m_image.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation) ;

    // Create Output Folder
    QDir dir ;
    if (!dir.mkpath(outputFolder)) return PM::OutputWriteError ;


    PM::Err err = PM::Ok ;

    int y=0 ;
    bool doney=false ;

    do {

        bool donex=false ;
        int x=0 ;

        do {
            // Calculate the width and height of the tile
            int sizex=tilesize ;
            int sizey=tilesize ;
            if (((x+1)*tilesize)>=width) {
                sizex=width-(x*tilesize) ;
                donex=true ;
            }
            if (((y+1)*tilesize)>=height) {
                sizey=height-(y*tilesize) ;
                doney=true ;
            }

            if (prog->isCancelled()) err = PM::OperationCancelled;
            else {
                prog->setValue(progstart+(100*((y*width)+(x*tilesize)))/(width*height)) ;
            }

            // Calculate the filename for the destination image
            QString outputfile = outputFolder + QString("/") + mask ;
            outputfile.replace(QString("%x"), QString::number(x)).replace(QString("%y"), QString::number(y)) ;

            // Extract the image from the requested face
            QImage dest(sizex, sizey, QImage::Format_ARGB32) ;
            dest = img.copy(x*tilesize, y*tilesize, sizex, sizey) ;

            // And save it
            if (!dest.save(outputfile)) err = PM::OutputWriteError ;

            x++ ;

        } while (!donex && err==PM::Ok) ;

        y++ ;

    } while (!doney && err==PM::Ok) ;

    return err ;
}


int Face::width()
{
    return m_image.width() ;
}

int Face::height()
{
    return m_image.height() ;
}

const Face Face::scaled(int w, int h)
{
    return (Face) (m_image.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)) ;
}

bool Face::save(QString filename)
{
    return m_image.save(filename) ;
}
