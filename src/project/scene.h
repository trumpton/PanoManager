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
// Scene
//

#ifndef SCENE_H
#define SCENE_H

#include <QList>
#include <QString>
#include "node.h"

class Scene
{
  private:
    bool m_invalid, m_empty, m_dirty ;
    int m_northOffset ;
    QString m_filename ;
    QString m_title ;
    QString m_id ;

    NodeList m_nodes ;
    Node m_invalidnode ;

  public:
    Scene(bool isinvalid=false);
    ~Scene() ;

    int northOffset() ;
    QString title() ;
    QString id() ;
    QString titleId() ;
    QString filename() ;
    QString folder() ;
    QString faceFilename(int face, bool highQuality=false) ;
    bool imageFilesExist(bool highQuality=false) ;

    bool isEmpty() ;
    bool isValid() ;
    bool isDirty() ;

    int nodeCount() ;
    NodeList& nodes() ;
    Node& nodeAt(int num) ;
    Node& node(QString id) ;
    bool removeNode(QString id) ;


    void setNorthOffset(int offset) ;
    void setFilename(QString sourceFilename) ;
    void setTitle(QString title) ;
    void markClean() ;
    void clear() ;
    void setId(QString id) ;
};

typedef QList<Scene> SceneList;

#endif // SCENE_H
