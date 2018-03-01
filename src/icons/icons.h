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
// Icons
//

#ifndef ICONTYPES_H
#define ICONTYPES_H


class Icon {

public:

    typedef enum {
        WInfo = 0,
        WExit,
        WLink000,
        WLink045,
        WLink090,
        WLink135,
        WLink180,
        WLink225,
        WLink270,
        WLink315,
        WMedia,
        WMusic,
        BInfo,
        BExit,
        BLink000,
        BLink045,
        BLink090,
        BLink135,
        BLink180,
        BLink225,
        BLink270,
        BLink315,
        BMedia,
        BMusic
    } IconType ;

    typedef enum {
        Info = 0,
        Exit,
        Link,
        Media,
        Music
    } Group ;

    const static unsigned int numTextures = 24 ;

public:
    static const char *textureFile(IconType num) ;
    static int textureOrientation(IconType num) ;
    static const char *menuFile(IconType num) ;
    static Group textureGroup(IconType num) ;

};


#endif // ICONTYPES_H
