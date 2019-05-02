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

#ifndef FACE_H
#define FACE_H

#include <QString>
#include <QImage>
#include <QObject>
#include "../errors/pmerrors.h"
#include "maptranslation.h"

class Face : public QObject, public QImage
{
    Q_OBJECT

private:
    bool m_abort ;

public:
    // Constructor
    explicit Face() ;
    explicit Face(const QImage& img) ;
    ~Face() ;

private:
    Face(const Face& other) ;
    Face& operator=(Face& rhs) ;

public:
    // Release memory
    void clear() ;

    // Build face 'f' of size 'size x size', from equirectangular image 'source'
    PM::Err build(MapTranslation& map, QImage source, int f, int size) ;

    // Export targetimageszie sized image made of tilessize sized tiles to outputFolder,
    // using mask to create individual files.
    PM::Err exportTiles(int targetimagesize, int tilesize, QString outputFolder, QString mask) ;

    // Copy Face to Face
    Face& operator=(const Face& d)
    {
        QImage::operator=(d);
        m_abort = d.m_abort;
        return *this;
    }

    // Copy Image to Face
    Face& operator=(const QImage& d)
    {
        QImage::operator=(d);
        return *this;
    }

signals:
    void progressUpdate(QString message) ;
    void percentUpdate(int percent) ;
    void abort() ;

public slots:
    void handleAbort() ;

};

#endif // FACE_H
