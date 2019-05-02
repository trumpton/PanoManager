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

#ifndef NODE_H
#define NODE_H

#include <QString>
#include <QList>
#include "../icons/icons.h"

class Node
{
private:
    bool m_invalid, m_dirty, m_empty ;
    QString m_id ;
    Icon::IconType m_type ;
    int m_lat, m_lon ;      // in millidegrees
    QString m_title ;
    QString m_description ;
    QString m_destId ;
    QString m_url ;
    int m_arrivalLat, m_arrivalLon ;        // in millidegrees

public:
    Node(bool isinvalid=false);
    ~Node() ;
    Node(const Node& rhs) ;
    Node& operator=(const Node& rhs) ;

public:
    Icon::IconType type() ;
    int lat() ;
    int lon() ;
    QString description() ;
    QString title() ;
    int arrivalLat() ;
    int arrivalLon() ;
    QString destId() ;
    QString url() ;
    QString id() ;

    bool isEmpty() ;
    bool isValid() ;
    bool isDirty() ;

    void setType(Icon::IconType type) ;

    bool isLink() ;
    bool isInfo() ;
    bool isMedia() ;
    bool isMusic() ;

    void setLat(int lat) ;
    void setLon(int lon) ;
    void setArrivalLat(int lat) ;
    void setArrivalLon(int lon) ;
    void setDestId(QString id) ;
    void setUrl(QString url) ;
    void setTitle(QString title) ;
    void setDescription(QString desc) ;
    void setId(QString id) ;
    void markClean();
    void clear() ;

    //int operator==(const Node &rhs) const;
    //int operator<(const Node &rhs) const;
};

typedef QList<Node> NodeList;

#endif // NODE_H
