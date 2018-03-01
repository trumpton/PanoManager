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

#include "../project/project.h"
#include "../icons/icons.h"

#include <QSettings>
#include <QFileInfo>

Project::Project() : m_invalidScene(true)
{
    clear() ;
}

Project::~Project()
{
    clear() ;
}

int Project::sceneCount()
{
    return m_scenes.count() ;
}

SceneList& Project::scenes()
{
    return m_scenes ;
}

Scene& Project::sceneAt(int num)
{
    if (num<0 || num>=m_scenes.count()) return m_invalidScene ;
    else return m_scenes[num] ;
}

Scene& Project::scene(QString id)
{
    for (int i=0; i<m_scenes.count(); i++) {
        if (m_scenes[i].id().compare(id)==0) return m_scenes[i] ;
    }
    return m_invalidScene ;
}

bool Project::removeScene(QString id)
{
    for (int i=0; i<m_scenes.count(); i++) {
        if (m_scenes[i].id().compare(id)==0) {
            m_scenes.removeAt(i) ;
            return true ;
        }
    }
    return false ;
}

bool Project::OpenProject(QString path)
{
    clear() ;
    m_projectpath = path ;
    QSettings project(path, QSettings::IniFormat) ;
    int numScenes = project.value("numScenes", (int)0).toInt() ;

    m_title = project.value("title", QString("")).toString() ;
    m_author = project.value("author", QString("")).toString() ;
    m_startingSceneId = project.value("startingScene", QString("")).toString() ;
    m_startingSceneLat = project.value("startingSceneLat", (int)0).toInt() ;
    m_startingSceneLon = project.value("startingSceneLon", (int)0).toInt() ;
    m_autoRotate = project.value("autoRotate", (int)5000).toInt() ;
    m_sceneFade = project.value("sceneFade", (int)2000).toInt() ;
    m_compass = project.value("compass", false).toBool() ;
    m_autoLoad = project.value("autoLoad", true).toBool() ;
    m_debug = project.value("debug", false).toBool() ;

    for (int s=0; s<numScenes; s++) {

        Scene newScene ;
        QString scenePrefix = QString("scene_") + QString::number(s) + QString("_") ;

        int numNodes = project.value(scenePrefix + "numNodes", (int)0).toInt() ;
        for (int n=0; n<numNodes; n++) {

            Node newNode ;
            QString nodePrefix = scenePrefix + QString("node_") + QString::number(n) + QString("_") ;

            newNode.setId(project.value(nodePrefix + "id", 0).toString()) ;
            int type = project.value(nodePrefix + "type", 0).toInt() ;
            if (type<0 || type>=Icon::numTextures) type=0 ;
            newNode.setType((Icon::IconType)type);
            newNode.setLat(project.value(nodePrefix + "lat", 0).toInt()) ;
            newNode.setLon(project.value(nodePrefix + "lon", 0).toInt()) ;
            newNode.setArrivalLat(project.value(nodePrefix + "arrivalLat", 0).toInt()) ;
            newNode.setArrivalLon(project.value(nodePrefix + "arrivalLon", 0).toInt()) ;
            newNode.setTitle(project.value(nodePrefix + "title", "").toString()) ;
            newNode.setDescription(project.value(nodePrefix + "description", "").toString()) ;
            newNode.setDestId(project.value(nodePrefix + "destId", 0).toString()) ;
            newNode.setUrl(project.value(nodePrefix + "url", "").toString()) ;
            newScene.nodes().append(newNode) ;
        }

        newScene.setFilename(project.value(scenePrefix + QString("imageFolder")).toString()) ;
        newScene.setTitle(project.value(scenePrefix + QString("sceneName")).toString()) ;
        newScene.setNorthOffset(project.value(scenePrefix + QString("northOffsetLon")).toInt()) ;
        newScene.setId(project.value(scenePrefix + QString("id")).toString()) ;
        m_scenes.append(newScene) ;

    }
    m_empty = false ;
    m_dirty = false ;
    return true ;
}

void Project::clear()
{
    m_title.clear() ;
    m_author.clear() ;
    m_startingSceneId.clear() ;
    m_startingSceneLat=0 ;
    m_startingSceneLon=0 ;
    m_autoRotate=5000 ;
    m_sceneFade=2000 ;
    m_compass=false ;
    m_autoLoad=true ;
    m_debug=false ;
    m_overwriteLibrary=false ;

    m_scenes.clear() ;
    m_dirty=false ;
    m_empty=true ;
}

bool Project::SaveProject(QString path)
{
    if (!isDirty() || isEmpty()) return true ;
    if (path.isEmpty()) path = m_projectpath ;
    m_projectpath = path ;
    QSettings project(path, QSettings::IniFormat) ;
    project.clear() ;

    int numScenes = m_scenes.count() ;
    project.setValue("numScenes", numScenes) ;

    project.setValue("title", m_title) ;
    project.setValue("author", m_author) ;
    project.setValue("startingScene", m_startingSceneId) ;
    project.setValue("startingSceneLat", m_startingSceneLat) ;
    project.setValue("startingSceneLon", m_startingSceneLon) ;
    project.setValue("autoRotate", m_autoRotate) ;
    project.setValue("sceneFade", m_sceneFade) ;
    project.setValue("compass", m_compass) ;
    project.setValue("autoLoad", m_autoLoad) ;
    project.setValue("debug", m_debug) ;

    for (int s=0; s<numScenes; s++) {

        QString scenePrefix = QString("scene_") + QString::number(s) + QString("_") ;
        Scene& sc = sceneAt(s) ;

        project.setValue(scenePrefix + "id", sc.id()) ;
        project.setValue(scenePrefix + QString("imageFolder"), sc.filename()) ;
        project.setValue(scenePrefix + QString("sceneName"), sc.title()) ;
        project.setValue(scenePrefix + QString("northOffsetLon"), sc.northOffset()) ;

        int numNodes = sc.nodeCount() ;
        project.setValue(scenePrefix + "numNodes", numNodes) ;

        for (int n=0; n<numNodes; n++) {

            Node& nd = sc.nodeAt(n) ;
            QString nodePrefix = scenePrefix + QString("node_") + QString::number(n) + QString("_") ;

            project.setValue(nodePrefix + "id", nd.id()) ;
            project.setValue(nodePrefix + "type", (int)nd.type()) ;
            project.setValue(nodePrefix + "lat", nd.lat()) ;
            project.setValue(nodePrefix + "lon", nd.lon()) ;
            project.setValue(nodePrefix + "arrivalLat", nd.arrivalLat()) ;
            project.setValue(nodePrefix + "arrivalLon", nd.arrivalLon()) ;
            project.setValue(nodePrefix + "title", nd.title()) ;
            project.setValue(nodePrefix + "description", nd.description()) ;
            project.setValue(nodePrefix + "destId", nd.destId()) ;
            project.setValue(nodePrefix + "url", nd.url()) ;

        }
    }
    m_dirty=false ;
    for (int i=0; i<m_scenes.count(); i++) m_scenes[i].markClean();
    return true ;
}


QString Project::title() { return m_title ; }
QString Project::author() { return m_author ; }
QString Project::startingSceneId() { return m_startingSceneId ; }
int Project::startingSceneLat() { return m_startingSceneLat ; }
int Project::startingSceneLon() { return m_startingSceneLon ; }
int Project::autoRotate() { return m_autoRotate ; }
int Project::sceneFade() { return m_sceneFade ; }
bool Project::compass() { return m_compass ; }
bool Project::autoLoad() { return m_autoLoad ; }
bool Project::debug() { return m_debug ; }
bool Project::overwriteLibrary() { return m_overwriteLibrary ; }
void Project::setTitle(QString title)
{
    if (m_title.compare(title)!=0) {
        m_title = title ;
        m_dirty=true ;
        m_empty = false ;
    }
}

void Project::setAuthor(QString author)
{
    if (m_author.compare(author)!=0) {
        m_author = author ;
        m_dirty=true ;
        m_empty = false ;
    }
}

void Project::setStartingScene(QString id, int lat, int lon)
{
    if (m_startingSceneId.compare(id)!=0 || m_startingSceneLat!=lat || m_startingSceneLon!=lon) {
        m_startingSceneId = id ;
        m_startingSceneLat = lat ;
        m_startingSceneLon = lon ;
        m_dirty=true ;
        m_empty = false ;
    }
}

void Project::setAutoRotate(int ms)
{
    if (m_autoRotate!=ms) {
        m_autoRotate = ms ;
        m_dirty=true ;
        m_empty = false ;
    }
}

void Project::setSceneFade(int ms)
{
    if (m_autoLoad!=ms) {
        m_autoLoad = ms ;
        m_dirty=true ;
        m_empty = false ;
    }
}

void Project::setCompass(bool yes)
{
    if (m_compass!=yes) {
        m_compass = yes ;
        m_dirty=true ;
        m_empty = false ;
    }
}

void Project::setAutoLoad(bool yes)
{
    if (m_autoLoad!=yes) {
        m_autoLoad = yes ;
        m_dirty=true ;
        m_empty = false ;
    }
}

void Project::setDebug(bool yes)
{
    if (m_debug!=yes) {
        m_debug = yes ;
        m_dirty=true ;
        m_empty = false ;
    }
}

void Project::setOverwriteLibrary(bool yes)
{
    m_overwriteLibrary = yes ;
}

bool Project::isDirty() {
    bool dirty = m_dirty ;
    for (int i=0; i<m_scenes.count(); i++) {
        dirty |= m_scenes[i].isDirty() ;
    }
    return dirty ;
}

bool Project::isEmpty() {
    return m_empty && m_scenes.count()==0 ;
}
