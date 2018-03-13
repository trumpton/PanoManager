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
#include <QObject>
#include <QString>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>

Face::Face() : QObject(0), QImage()
{
    m_abort = false ;
}

Face::Face(const QImage& img) : QObject(0), QImage(img)
{
    m_abort = false ;
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
    *this = QImage(1, 1, QImage::Format_ARGB32) ;
}


////////////////////////////////////////////////////////////////////
/// \brief Face::build
/// \param source
/// \param f
/// \param size
/// \return
///

//!!TODO Percent Update

PM::Err Face::build(MapTranslation &map, QImage source, int f, int size)
{
    if (source.isNull()) return PM::InputNotDefined ;

    m_abort = false ;

    int srcx = source.width() ;
    int srcy = source.height() ;
    int dstxy = size ;

    PM::Err err = map.start(f, srcx, srcy, dstxy) ;
    if (err!=PM::Ok) return err ;

    emit(progressUpdate(QString("Building Face: ") + QString::number(f))) ;

    *this = QImage(dstxy, dstxy, QImage::Format_ARGB32) ; // Set Image Size
    if (width()!=dstxy || height()!=dstxy) err=PM::OutOfMemory ;

    MapCoordinate *coord ;
    unsigned long int iteration=0 ;

    do {

        coord=map.next() ;
        iteration++ ;

        if (coord->dstx==0) {
            QCoreApplication::processEvents();
            emit(percentUpdate((iteration*100)/(dstxy*dstxy)));
            if (m_abort) { err = PM::OperationCancelled ; }
        }

        if (coord->dstx>dstxy || coord->dsty>dstxy) {
            // dstx=22496, dsty=430, dstxy=512 ERR HERE !!!!!!!!!
            err = PM::InvalidMapTranslation ;
        }
        QRgb pix= source.pixel(coord->srcx, coord->srcy) ;
        setPixel(coord->dsty, coord->dstx, pix) ;

    } while (coord->face>=0 && iteration<=(dstxy*dstxy)) ;

    if (coord->face>=0) {
        err = PM::InvalidMapTranslation ;
    }

    map.end() ;
    return err ;
}


////////////////////////////////////////////////////////////////////
/// \brief Face::exportTiles
/// \param targetimagesize
/// \param tilesize
/// \param outputFolder
/// \param mask
/// \return
///

//!!TODO Percent Update

PM::Err Face::exportTiles(int targetimagesize, int tilesize, QString outputFolder, QString mask)
{
    m_abort = false ;
    emit(QString("Exporting Tiles for image size: ") + QString::number(targetimagesize) +
            QString("x") + QString::number(targetimagesize)) ;

    if (outputFolder.isEmpty()) return PM::OutputNotDefined ;
    if (targetimagesize<=0) return PM::InvalidTargetImageSize ;

    int width = targetimagesize ;
    int height = targetimagesize ;

    QImage img = scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation) ;

    // Create Output Folder
    QDir dir ;
    if (!dir.mkpath(outputFolder)) return PM::OutputWriteError ;


    PM::Err err = PM::Ok ;

    int y=0 ;
    bool doney=false ;

    do {

        bool donex=false ;
        int x=0 ;

        QCoreApplication::processEvents();
        if (m_abort) { err = PM::OperationCancelled ; }

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

void Face::handleAbort()
{
    m_abort = true ;
    emit(abort()) ;
}
