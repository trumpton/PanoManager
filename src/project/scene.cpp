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

#include "scene.h"
#include <QUuid>
#include <QFileInfo>

Scene::Scene(bool isinvalid) : m_invalidnode(true)
{
    m_invalid = isinvalid ;
    clear() ;
}

Scene::~Scene()
{
}

Scene::Scene(const Scene& rhs)
{
    *this = rhs ;
}

Scene& Scene::operator=(const Scene& rhs)
{
    m_invalid = rhs.m_invalid ;
    m_empty = rhs.m_empty ;
    m_dirty = rhs.m_dirty ;
    m_northOffset = rhs.m_northOffset ;
    m_filename = rhs.m_filename ;
    m_title = rhs.m_title ;
    m_id = rhs.m_id ;
    m_nodes = rhs.m_nodes ;
    m_invalidnode = rhs.m_invalidnode ;
    return *this ;
}


int Scene::northOffset()
{
    return m_northOffset ;
}


QString Scene::filename()
{
    return m_filename ;
}

QString Scene::folder()
{
    QFileInfo filename(m_filename) ;
    return filename.canonicalPath() + "/" + filename.baseName() ;
}

QString Scene::faceFilename(int face, bool highQuality)
{
    QString name ;
    name = folder() + "/" + "face00" + QString::number(face) ;
    if (!highQuality) name = name + "_preview" ;
    name = name + ".png" ;
    return name ;
}

bool Scene::imageFilesExist(bool highQuality)
{
    int exists = true ;
    for (int i=0; i<6; i++) {
        QString filename = faceFilename(i, highQuality) ;
        QFileInfo f(filename) ;
        if (!f.exists() || f.size()<1024) exists=false ;
    }
    return exists ;
}

QString Scene::title()
{
    return m_title ;
}

QString Scene::id()
{
    return m_id ;
}

QString Scene::titleId()
{
    // TODO: use regexp to replace all non a-z characters with ""
    return m_title.replace(" ","").replace("\t", "").replace("\\","").replace("/","").replace(":","") ;
}

bool Scene::isEmpty()
{
    return m_empty ;
}

bool Scene::isValid()
{
    return !m_invalid ;
}

bool Scene::isDirty()
{
    bool dirty = false ;
    for (int i=0; i<m_nodes.count(); i++) {
        Node *node = (Node *)&m_nodes.at(i) ;
        dirty |= node->isDirty() ;
    }
    return m_dirty || dirty ;
}

NodeList& Scene::nodes()
{
    return m_nodes ;
}

int Scene::nodeCount()
{
    return m_nodes.count() ;
}

Node& Scene::nodeAt(int num)
{
    if (num<0 || num>=m_nodes.count()) return m_invalidnode ;
    return (Node&)m_nodes.at(num) ;
}

Node& Scene::node(QString id)
{
    for (int i=0; i<m_nodes.count(); i++) {
        if (m_nodes[i].id().compare(id)==0) return m_nodes[i] ;
    }
    return m_invalidnode ;
}

bool Scene::removeNode(QString id)
{
    for (int i=0; i<m_nodes.count(); i++) {
        if (m_nodes[i].id().compare(id)==0) {
            m_nodes.removeAt(i) ;
            return true ;
        }
    }
    return false ;
}

void Scene::setNorthOffset(int offset)
{
    if (m_invalid) return ;
    m_northOffset = offset ;
    m_empty=false ;
    m_dirty=true ;
}

void Scene::setFilename(QString sourceFilename)
{
    if (m_invalid) return ;
    m_filename = sourceFilename ;
    m_empty=false ;
    m_dirty=true ;
}

void Scene::setTitle(QString title)
{
    if (m_invalid) return ;
    m_title = title ;
    m_empty=false ;
    m_dirty=true ;
}

void Scene::setId(QString id)
{
    if (m_invalid || id.isEmpty() || id.length()<10) return ;
    m_id = id.replace("{","").replace("}","").replace("-","") ;
}

void Scene::markClean()
{
    m_dirty = false ;
    for (int i=0; i<m_nodes.count(); i++) {
        m_nodes[i].markClean();
    }
}

void Scene::clear()
{
    QUuid uuid ;
    if (m_invalid)
        setId(uuid.toString()) ;
    else
        setId(uuid.createUuid().toString()) ;

    m_northOffset = 0 ;
    m_filename.clear() ;
    m_title.clear() ;
    m_nodes.clear() ;

    m_empty = true ;
    m_dirty = false ;

}
