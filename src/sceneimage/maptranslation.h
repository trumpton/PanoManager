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
// Map Translation
//

#ifndef MAPTRANSLATION_H
#define MAPTRANSLATION_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QDataStream>
#include "../errors/pmerrors.h"

typedef struct {
    char face ;                  // Cubemap destination face (-1 on error)
    unsigned short dstx, dsty ;  // Cubemap destination face coordinates
    unsigned short srcx ;        // equirectangular x coordinate: srcx + remx/100
    unsigned char remx ;
    unsigned short srcy ;        // equirectangular y coordinate: srcy + remy/000
    unsigned char remy ;
} MapCoordinate ;


class MapTranslation : public QObject
{
    Q_OBJECT

private:
    bool m_abort ;

    // Input files and data streams
    QFile m_file ;
    QDataStream m_in ;

    // Characteristics of input and output images
    int m_srcx, m_srcy, m_dstxy ;

    // Save Folder
    QString m_savefolder ;

    // Current face (0-5) and fileface (0-1)
    int m_face ;
    int m_fileface ;

    // Current position in the stream
    unsigned short m_x, m_y ;

    // Returned coordinates for each face
    MapCoordinate m_coords ;

    QString mapPath(int type, int srcx, int srcy, int dstxy) ;

    // Open the files
    PM::Err openFile(int face, unsigned short srcx, unsigned short srcy, unsigned short dstxy) ;

public:
    MapTranslation();

private:
    MapTranslation(MapTranslation &other) ;
    MapTranslation& operator=(const MapTranslation &rhs) ;

public:

    int srcx() ;
    int srcy() ;
    int dstxy() ;

    // Returns true if the requested map exists
    bool exists(unsigned short srcx, unsigned short srcy, unsigned short dstxy) ;

    // Build a new map, and save in the user's application cache folder
    PM::Err build(unsigned short srcx, unsigned short srcy, unsigned short dstxy) ;

    // Start a new map translation (the map must have already been built with build)
    PM::Err start(int face, unsigned short srcx, unsigned short srcy, unsigned short dstxy) ;

    // Get the next coordinate mapping for face f from cache files
    // Returned face=-1 on end of file or error
    MapCoordinate* next() ;

    // Finish the map translation, and close cache files
    bool end() ;

public slots:
    void handleAbort() ;

signals:
    void progressUpdate(QString message) ;
    void percentUpdate(int percent) ;

};

#endif // MAPTRANSLATION_H
