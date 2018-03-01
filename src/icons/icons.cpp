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

#include "icons.h"

typedef struct {
    Icon::Group group ;
    int textureIndex ;
    int textureOrientation ;
} icontype ;


const char * iconMenuTextures[] = {
        ":/texture/menu-info.png",  // Icon::Info
        ":/texture/menu-exit.png",  // Icon::Exit
        ":/texture/menu-link.png",  // Icon::Link
        ":/texture/menu-media.png", // Icon::Media
        ":/texture/menu-music.png"  // Icon::Music
} ;

const char * iconTextures[12] = {
        ":/texture/info-white.png",  // 0
        ":/texture/door-white.png",  // 1
        ":/texture/link0-white.png", // 2
        ":/texture/link45-white.png",// 3
        ":/texture/media-white.png", // 4
        ":/texture/music-white.png", // 5
        ":/texture/info-black.png",  // 6
        ":/texture/door-black.png",  // 7
        ":/texture/link0-black.png", // 8
        ":/texture/link45-black.png",// 9
        ":/texture/media-black.png", // 10
        ":/texture/music-black.png"  // 11
} ;

static constexpr icontype iconList[] = {
        { Icon::Info,  0, 0 },  // Icon::WInfo
        { Icon::Exit,  1, 0 },  // Icon::WExit
        { Icon::Link, 2, 0 },   // Icon::WLink000
        { Icon::Link, 3, 0 },   // Icon::WLink045
        { Icon::Link, 2, 90 },  // Icon::WLink090
        { Icon::Link, 3, 90 },  // Icon::WLink135
        { Icon::Link, 2, 180 }, // Icon::WLink180
        { Icon::Link, 3, 180 }, // Icon::WLink225
        { Icon::Link, 2, 270 }, // Icon::WLink270
        { Icon::Link, 3, 270 }, // Icon::WLink315
        { Icon::Media,  4, 0 }, // Icon::WMedia
        { Icon::Music,  5, 0 }, // Icon::WMusic
        { Icon::Info,  6, 0 },  // Icon::BInfo
        { Icon::Exit,  7, 0 },  // Icon::Exit
        { Icon::Link, 8, 0 },   // Icon::BLink000
        { Icon::Link, 9, 0 },   // Icon::BLink045
        { Icon::Link, 8, 90 },  // Icon::BLink090
        { Icon::Link, 9, 90 },  // Icon::BLink135
        { Icon::Link, 8, 180 }, // Icon::BLink180
        { Icon::Link, 9, 180 }, // Icon::BLink225
        { Icon::Link, 8, 270 }, // Icon::BLink270
        { Icon::Link, 9, 270 }, // Icon::BLink315
        { Icon::Media, 10, 0 }, // Icon::BMedia
        { Icon::Music, 11, 0 }  // Icon::BMusic
} ;


const char *Icon::textureFile(IconType num) {
        if (num>sizeof(iconList)) return iconTextures[0] ;
        return iconTextures[iconList[(int)num].textureIndex] ;
}

int Icon::textureOrientation(IconType num) {
        if (num>sizeof(iconList)) return iconList[0].textureOrientation ;
        return iconList[(int)num].textureOrientation ;
}

const char *Icon::menuFile(IconType num) {
        if (num>sizeof(iconList)) return iconMenuTextures[0] ;
        return iconMenuTextures[(int)iconList[(int)num].group] ;
}

Icon::Group Icon::textureGroup(IconType num) {
        if (num>sizeof(iconList)) return iconList[0].group ;
        return iconList[(int)num].group ;
}

