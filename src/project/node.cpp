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
// Node
//

#include "node.h"
#include <QUuid>
#include "../icons/icons.h"

Node::Node(bool isinvalid)
{
    m_invalid = isinvalid ;
    clear() ;
}

Node::~Node()
{
}

Icon::IconType Node::type()
{
    return m_type ;
}

int Node::lat()
{
    return m_lat ;
}

int Node::lon()
{
    return m_lon ;
}

int Node::arrivalLat()
{
    return m_arrivalLat ;
}

int Node::arrivalLon()
{
    return m_arrivalLon ;
}

QString Node::title()
{
    return m_title ;
}

QString Node::description()
{
    return m_description ;
}

QString Node::url()
{
    return m_url ;
}

QString Node::destId()
{
    return m_destId ;
}

QString Node::id()
{
    return m_id ;
}

bool Node::isEmpty()
{
    return m_empty ;
}

bool Node::isValid()
{
    return !m_invalid ;
}

bool Node::isDirty()
{
    return m_dirty ;
}

void Node::setType(Icon::IconType type)
{
    if (m_invalid) return ;
    m_type = type ;
    m_empty=false ;
    m_dirty=true ;
}

bool Node::isLink()
{
    Icon::Group g = Icon::textureGroup(m_type) ;
    return (g == Icon::Link || g == Icon::Exit) ;
}

bool Node::isInfo()
{
    Icon::Group g = Icon::textureGroup(m_type) ;
    return (g == Icon::Info) ;
}

bool Node::isMedia()
{
    Icon::Group g = Icon::textureGroup(m_type) ;
    return (g == Icon::Media) ;
}

bool Node::isMusic()
{
    Icon::Group g = Icon::textureGroup(m_type) ;
    return (g == Icon::Music) ;
}

void Node::setLat(int lat)
{
    if (m_invalid) return ;
    m_lat = lat ;
    m_empty=false ;
    m_dirty=true ;
}

void Node::setLon(int lon)
{
    if (m_invalid) return ;
    m_lon = lon ;
    m_empty=false ;
    m_dirty=true ;
}

void Node::setArrivalLat(int lat)
{
    if (m_invalid) return ;
    m_arrivalLat = lat ;
    m_empty=false ;
    m_dirty=true ;
}

void Node::setArrivalLon(int lon)
{
    if (m_invalid) return ;
    m_arrivalLon = lon ;
    m_empty=false ;
    m_dirty=true ;
}

void Node:: setDestId(QString id)
{
    if (m_invalid || id.isEmpty() || id.length()<10) return ;
    m_destId = id.replace("{","").replace("}","").replace("-","") ;
    m_empty=false ;
    m_dirty=true ;
}

void Node::setUrl(QString url)
{
    if (m_invalid) return ;
    m_url = url ;
    m_empty=false ;
    m_dirty=true ;
}

void Node::setDescription(QString desc)
{
    if (m_invalid) return ;
    m_description = desc ;
    m_empty=false ;
    m_dirty=true ;
}

void Node::setTitle(QString title)
{
    if (m_invalid) return ;
    m_title = title ;
    m_empty=false ;
    m_dirty=true ;
}

void Node::setId(QString id)
{
    if (m_invalid || id.isEmpty() || id.length()<10) return ;
    m_id = id.replace("{","").replace("}","").replace("-","") ;
}

void Node::markClean()
{
    m_dirty = false ;
}

void Node::clear()
{
    QUuid uuid ;
    if (m_invalid)
        setId(uuid.toString()) ;
    else
        setId(uuid.createUuid().toString()) ;

    m_empty = true ;
    m_dirty = true ;

    m_type = Icon::WInfo ;
    m_lat = 0 ;
    m_lon = 0 ;
    m_description.clear() ;
    m_destId.clear() ;
    m_arrivalLat = 0 ;
    m_arrivalLon = 0 ;
    m_url = "" ;

}
