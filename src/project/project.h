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
// Project
//

#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QList>
#include "scene.h"


class Project
{
private:
    QString m_title, m_author, m_startingSceneId ;
    int m_startingSceneLat, m_startingSceneLon ;
    int m_autoRotate, m_sceneFade ;
    bool m_compass, m_autoLoad, m_debug ;

    QString m_projectpath ;
    Scene m_invalidScene ;
    SceneList m_scenes ;
    bool m_dirty ;
    bool m_empty ;
    bool m_overwriteLibrary ;

public:

    Project();
    ~Project() ;

private:
    Project(const Project& other) ;
    Project& operator=(const Project& rhs) ;

public:
    void clear() ;

    bool OpenProject(QString path) ;
    bool SaveProject(QString path = QString("")) ;

    int sceneCount() ;
    SceneList& scenes() ;
    Scene& sceneAt(int num) ;
    Scene& scene(QString id) ;
    bool removeScene(QString id) ;

    bool isDirty() ;
    bool isEmpty() ;

    QString title() ;
    QString author() ;
    QString startingSceneId() ;
    int startingSceneLat() ;
    int startingSceneLon() ;
    int autoRotate() ;
    int sceneFade() ;
    bool compass() ;
    bool autoLoad() ;
    bool debug() ;
    bool overwriteLibrary() ;

    void setTitle(QString title) ;
    void setAuthor(QString author) ;
    void setStartingScene(QString id, int lat, int lon) ;
    void setAutoRotate(int ms) ;
    void setSceneFade(int ms) ;
    void setCompass(bool yes) ;
    void setAutoLoad(bool yes) ;
    void setDebug(bool yes) ;
    void setOverwriteLibrary(bool yes) ; // Note: Intentionally not saved

};

#endif // PROJECT_H
