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

#ifndef SCENEIMAGE_H
#define SCENEIMAGE_H

#include <QString>
#include <QObject>
#include "../errors/pmerrors.h"
#include "../sceneimage/face.h"
#include "maptranslation.h"

class SceneImage : public QObject
{
    Q_OBJECT

private:
    bool m_abort ;
    QString m_filename ;
    QString m_facedir ;
    Face m_faces[6] ;
    bool m_ispreview ;

    // Counters used to report % progress
    int m_loadMax ;
    int m_loadPos ;
    int m_buildLoadFace ;
    int m_buildLoadSteps ;

    PM::Err buildFaces(bool buildpreview=false) ;
    PM::Err loadFaces(bool loadpreview, bool scaleforpreview = true) ;

public:
    SceneImage();

private:
    SceneImage(const SceneImage& other) ;
    SceneImage& operator=(const SceneImage &rhs) ;

public:
    void clear() ;
    bool facesExist(QString imagefile) ;
    bool previewExists(QString imagefile) ;

    PM::Err loadImage(QString imagefile, bool loadpreview, bool buildpreview, bool scaleforpreview, bool buildonly) ;
    Face& getFace(int n) ;

    PM::Err exportVerticalPreview(int width, int *sequence, QString filename) ;

signals:
    void progressUpdate(QString message) ;
    void abort() ;
    void percentUpdate(int percent) ;

public slots:
    void handleProgressUpdate(QString message) ;
    void handleAbort() ;
    void handlePercentUpdate(int percent) ;

};

#endif // SCENEIMAGE_H
