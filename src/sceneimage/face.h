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
#include "../dialogs/progress/progressdialog.h"
#include "../errors/pmerrors.h"

class Face
{
private:
    QImage m_image ;
    void setImage(QImage& img) ;

public:
    // Constructor
    Face() ;
    Face(const QImage& img) ;
    ~Face() ;

    // Load Image
    bool load(QString filename) ;

    // Access Image Data
    QImage& image() ;

    int width() ;
    int height() ;
    const Face scaled(int w, int h) ;
    bool save(QString filename) ;

    // Release memory
    void clear() ;

    // Build face 'f' of size 'size x size', from equirectangular image 'source'
    // Advanced prog on by 100
    PM::Err build(ProgressDialog *prog, QImage source, int f, int size) ;

    // Export targetimageszie sized image made of tilessize sized tiles to outputFolder,
    // using mask to create individual files.
    // Advances prog on by 100
    PM::Err exportTiles(ProgressDialog *prog, int targetimagesize, int tilesize, QString outputFolder, QString mask) ;

    Face& operator= (const Face& other) { m_image = other.m_image ; return *this ; }
    Face& operator= (const QImage& other) { m_image = other ; return *this ; }

};

#endif // FACE_H
