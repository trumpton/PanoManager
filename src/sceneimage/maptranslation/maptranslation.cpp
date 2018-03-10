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

#include "maptranslation.h"
#include <QFile>
#include <QDataStream>
#include <QProgressBar>
#include <QApplication>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDir>
#include <QtGlobal>

#ifndef M_PI
#define M_PI 3.141592654
#define M_PI_2 M_PI/2
#define M_PI_4 M_PI/4
#endif
//
// maptranslation creates equirectangular to cubic maps, which are files stored
// in the application cache directory, mapping from a source equirectangular image
// to 6 cube face images.
//
// Maps are stored by source X/Y and destination XY sizes.
//
// The map translation creates only two maps - one (type 0) for the faces
// front, left, back, right, and the other (type 1) for the axes up and down.
//
// These maps are then parsed when read.
//

//
// Equirecangular SRC Image is in the following format:
//
//  <------ srcx ------>
//
//  UUUUUUUUUUUUUUUUUUUU   ^
//  UUUUUUUUUUUUUUUUUUUU   |
//  BB LLLL FFFF RRRR BB   |
//  BB LLLL FFFF RRRR BB  srcy
//  BB LLLL FFFF RRRR BB   |
//  DDDDDDDDDDDDDDDDDDDD   |
//  DDDDDDDDDDDDDDDDDDDD   V
//

//
// Output 6*Cube DST images are in the following format:
//
//                               +---------+
//                               |         |
//           <- dstxy ->         |    U    |
//                               |  type1  |
// +---------+---------+---------+---------+    ^
// |         |         |         |         |    |
// |    L    |    F    |    R    |    B    |  dstxy
// |         |  type0  |         |         |    |
// +---------+---------+---------+---------+    V
//                               |         |
//                               |    D    |
//                               |         |
//                               +---------+
//

//
// Usage:
//
//  PM::Err err ;
//  ProgressDialog dlg ;
//  QImage src("eequirect.jpg") ;
//  unsigned short srcx = src.width(), srcy=src.height(), dstxy = 1024 ;
//  for (int f=0; f<6; f++) {
//    QImage dest(dstxy, dstxy) ;
//    MapTranslation trans ;
//    MapCoordinate *coord ;
//    trans.start(&prog, 0, srcx, srcy, dstxy) ;
//    coord = trans.next() ;
//    while (coord.face>0) {
//      dst.setPixel(coord.dstx, coord.dsty, src.getPixel(coord.srcx, coord.srcy)) ;
//      coord = trans.next() ;
//    }
//    dest.save(QString("face")+QString::number(f)+Qstring(".png") ;
//    trans.end() ;
//  }
//

#define MAGIC (unsigned short)0x13FE

MapTranslation::MapTranslation() : QObject(0)
{
    m_abort = false ;
    m_dstxy=0 ;
    m_savefolder = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) ;
    QDir dir(m_savefolder) ;
    if (!dir.exists()) {
        dir.mkpath(".") ;
    }
}

int MapTranslation::srcx() { return m_srcx ; }
int MapTranslation::srcy() { return m_srcy ; }
int MapTranslation::dstxy() { return m_dstxy ; }

QString MapTranslation::mapPath(int face, int srcx, int srcy, int dstxy)
{
    QString ext ;
    if (face<4) {
        ext = "_face.map" ;
    } else {
        ext = "_axis.map" ;
    }
    return  m_savefolder + "/translationmatrix_" + QString::number(srcx) + "x" + QString::number(srcy) + "_" + QString::number(dstxy) + ext ;
}

PM::Err MapTranslation::start(int face, unsigned short srcx, unsigned short srcy, unsigned short dstxy)
{

    m_abort = false ;
    end() ;

    PM::Err err = PM::Ok ;

    if (face<0 || face>5) err = PM::InputNotDefined ;
    if (srcx==0 || srcy==0 || dstxy==0) err = PM::InputNotDefined ;
    if (err==PM::Ok && m_savefolder.isEmpty()) err = PM::OutputNotDefined ;
    if (err==PM::Ok) {

        // Attempt to open
        emit(progressUpdate("Loading Translation Map")) ;
        err = openFile(face, srcx, srcy, dstxy) ;

        if (err!=PM::Ok) {
            // Open failed, so attempt to build then open
            emit(progressUpdate("Building Translation Map")) ;
            err = buildAndSave(srcx, srcy, dstxy) ;
            if (err==PM::Ok) {
                    emit(progressUpdate("Loading Translation Map")) ;
                    err = openFile(face, srcx, srcy, dstxy)  ;
            }
        }
    }
    return err ;
}

MapCoordinate *MapTranslation::next()
{
    if (m_dstxy==0 || m_y>=m_dstxy) {

        m_coords.face = -1 ;
        m_coords.srcx= 0 ;
        m_coords.srcy= 0 ;
        m_coords.remx= 0 ;
        m_coords.remy= 0 ;
        m_coords.dstx= 0 ;
        m_coords.dsty= 0 ;
        return &m_coords ;

    } else {

        m_in >> m_coords.srcx ;
        m_in >> m_coords.srcy ;
        m_in >> m_coords.remx ;
        m_in >> m_coords.remy ;

        m_coords.face = m_face ;
        m_coords.dstx = m_x ;
        m_coords.dsty = m_y ;
        m_x++ ;

        if (m_x>=m_dstxy) {
            m_x=0 ;
            m_y++ ;
        }

        // Adjust for face 1,2,3 (right, back, left)
        // Adjust for face 5 (bottom)

        switch (m_face) {
        case 0:
            // Front is the default, no adjustments required
            break ;
        case 1:
            // Right
            m_coords.srcx = m_coords.srcx + m_srcx/4 ;
            break ;
        case 2:
            // Rear
            m_coords.srcx = m_coords.srcx + m_srcx/2 ;
            if (m_coords.srcx>m_srcx) m_coords.srcx-=m_srcx ;
            break ;
        case 3:
            // Left
            m_coords.srcx = m_coords.srcx - m_srcx/4 ;
            break ;
        case 4:
            // Up is the default, no adjustments required
            break ;
        case 5:
            // Down
            m_coords.srcy = m_srcy - m_coords.srcy ;
            m_coords.remy = 100 - m_coords.remy ;
            m_coords.dstx = m_dstxy - m_x ;
            break ;
        }

        return &m_coords ;

    }
}

bool MapTranslation::end()
{
    if (m_file.isOpen()) m_file.close() ;
    m_x=0 ;
    m_y=0 ;
    m_dstxy=0 ;
    return true ;
}


// Map0 is for faces 0-3, Map1 is for faces 4,5
PM::Err MapTranslation::openFile(int face, unsigned short srcx, unsigned short srcy, unsigned short dstxy)
{
    if (face<0 || face>5) return PM::InputNotDefined ;
    unsigned short filemagic, filesrcx, filesrcy, filedstxy ;
    unsigned short fileface ;

    PM::Err err = PM::Ok ;

    m_file.setFileName(mapPath(face, srcx, srcy, dstxy));
    if (!m_file.open(QIODevice::ReadOnly)) {
        err = PM::InvalidMapTranslation ;
    }

    m_in.setDevice(&m_file) ;
    m_in >> filemagic ;
    m_in >> filesrcx ;
    m_in >> filesrcy ;
    m_in >> filedstxy ;
    m_in >> fileface ;
    m_x = 0 ;
    m_y = 0 ;

    if (filemagic!=MAGIC || filesrcx!=srcx || filesrcy!=srcy || filedstxy!=dstxy || fileface<0 || fileface>1) {
        err = PM::InvalidMapTranslation ;
    }
    if ( fileface==0 && face>3) {
        err = PM::InvalidMapTranslation ;
    }
    if (fileface==1 && face<4) {
        err = PM::InvalidMapTranslation ;
    }

    m_srcx = filesrcx ;
    m_srcy = filesrcy ;
    m_dstxy = filedstxy ;
    m_face = face ;
    m_fileface = fileface ;

    if (err!=PM::Ok)
        if (m_file.isOpen()) m_file.close() ;

    return err ;
}

// Map0 is for the flbr, and map1 is for ud
// Create the maps for the six faces, from equirectangular to cubemap
// Based on: https://stackoverflow.com/questions/29678510/convert-21-equirectangular-panorama-to-cube-map
// Advances prog on by 100
PM::Err MapTranslation::buildAndSave(unsigned short srcx, unsigned short srcy, unsigned short dstxy) {

    if (m_savefolder.isEmpty()) return PM::OutputNotDefined ;

    QFile file[2] ;
    QDataStream out[2] ;

    m_abort = false ;
    m_srcx = srcx ;
    m_srcy = srcy ;
    m_dstxy = dstxy ;

    PM::Err err = PM::Ok ;

    for (int f=0; err==PM::Ok && f<=1; f++) {
        QString fileName = mapPath(f*4, srcx, srcy, dstxy) ;
        file[f].setFileName(fileName);

        if (!file[f].open(QIODevice::WriteOnly)) {
            err = PM::InvalidMapTranslation ;
        } else {
            out[f].setDevice(&file[f]) ;
            out[f] << MAGIC ;
            out[f] << (unsigned short)srcx ;
            out[f] << (unsigned short)srcy ;
            out[f] << (unsigned short)dstxy ;
            out[f] << (unsigned short)f ;
        }
    }

    double pi = M_PI ;
    double pi_2 = M_PI_2 ;
    double pi_4 = M_PI_4 ;

    double inWidth = srcx ;
    double inHeight = srcy ;
    double inWidthMinusOne = (inWidth - 1) ;
    double inHeightMinusOne = (inHeight - 1) ;
    double width = dstxy ;
    double height = dstxy ;
    unsigned short iwidth = dstxy ;
    unsigned short iheight = dstxy ;


    // Calculate adjacent (ak) and opposite (an) of the
    // triangle that is spanned from the sphere center
    //to our cube face.
    double an = sin(pi_4);
    double ak = cos(pi_4);

    double ftu[2],  ftv[2] ;
    ftu[0] =  0;    ftv[0] = 0;      // 0 - Front
    //ftu[0] =  pi_2; ftv[0] = 0;      // 1 - Right
    //ftu[0] =  pi;   ftv[0] = 0;      // 2 - Back
    //ftu[0] = -pi_2; ftv[0] = 0;      // 3 - Left
    ftu[1] =  pi_2; ftv[1] = -pi_2;  // 4 - Top
    //ftu[1] =  pi_2; ftv[1] = pi_2;   // 5 - Bottom

    double d, dak ;
    double nx, ny, nxsquared, nysquared ;
    double u[2], v[2] ;

    // For each point in the target image,
    // calculate the corresponding source coordinates.
    for(unsigned short y = 0; err==PM::Ok && y < iheight; y++) {

        // Map face pixel coordinates to [-1, 1] on plane
        nx = (double)y / height - 0.5f;
        nx *= 2;
        // Map [-1, 1] plane coords to [-an, an] thats the coordinates in respect to a unit sphere that contains our box.
        nx *= an;
        nxsquared = nx * nx ;

        QCoreApplication::processEvents();
        if (m_abort) { err = PM::OperationCancelled ; }

        for(unsigned short x = 0; x < iwidth; x++) {

            // Map face pixel coordinates to [-1, 1] on plane
            ny = (double)x / width - 0.5f;
            ny *= 2;
            // Map [-1, 1] plane coords to [-an, an] thats the coordinates in respect to a unit sphere that contains our box.
            ny *= an;
            nysquared = ny * ny ;

            // Project from plane to sphere surface.

            // Center faces
            u[0] = atan2(nx, ak);
            v[0] = atan2(ny * cos(u[0]), ak);

            // Top and Bottom Faces
            d = sqrt(nxsquared + nysquared);
            dak = atan2(d, ak) ;

            v[1] = ftv[1] + dak ;
            u[1] = atan2(-ny, nx) ;
            u[1] = u[1] + ftu[1];

            for (int k=0; k<=1; k++) {

                // Map from angular coordinates to [-1, 1], respectively.
                u[k] = u[k] / pi;
                v[k] = v[k] / pi_2 ;

                // Warp around, if our coordinates are out of bounds.
                while (v[k] < -1) {  v[k] += 2;  u[k] += 1; }
                while (v[k] > 1) { v[k] -= 2;  u[k] += 1; }
                while (u[k] < -1) { u[k] += 2; }
                while (u[k] > 1) { u[k] -= 2; }

                // Map from [-1, 1] to in texture space
                 u[k] = u[k] / 2.0f + 0.5f;
                 v[k] = v[k] / 2.0f + 0.5f;

                 u[k] = u[k] * inWidthMinusOne ;
                 v[k] = v[k] * inHeightMinusOne ;

                 unsigned short us = (unsigned short)u[k] ;
                 unsigned short vs = (unsigned short)v[k] ;

                 out[k] << (unsigned short)us ;
                 out[k] << (unsigned short)vs ;
                 out[k] << (unsigned char)((u[k]-(double)us)*100) ;
                 out[k] << (unsigned char)((v[k]-(double)vs)*100) ;

                 // if out[k].writeerror() err=PM::WriteError
            }
        }
    }

    for (int f=0; f<=1; f++) {
        file[f].close() ;
    }

    return err ;
}

void MapTranslation::handleAbort()
{
    m_abort = true ;
}
