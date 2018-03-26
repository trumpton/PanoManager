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
    Icon::IconType type ;
    const char * icontexture ;
    const char * iconname ;
} icontype ;

const char * iconMenuTextures[] = {
        ":/texture/menu-start.png", // Icon::GStart
        ":/texture/menu-info.png",  // Icon::GInfo
        ":/texture/menu-exit.png",  // Icon::GExit
        ":/texture/menu-link.png",  // Icon::GLink
        ":/texture/menu-media.png", // Icon::GMedia
        ":/texture/menu-music.png"  // Icon::GMusic
} ;

static constexpr icontype iconList[] = {
  { Icon::GStart, Icon::Start,    ":/texture/start.png",         "start" },
  { Icon::GInfo,  Icon::WInfo,    ":/texture/info-white.png",    "winfo" },
  { Icon::GExit,  Icon::WExit,    ":/texture/door-white.png",    "wexit" },
  { Icon::GLink,  Icon::WLink000, ":/texture/link000-white.png", "wlink000" },
  { Icon::GLink,  Icon::WLink045, ":/texture/link045-white.png", "wlink045" },
  { Icon::GLink,  Icon::WLink090, ":/texture/link090-white.png", "wlink090" },
  { Icon::GLink,  Icon::WLink135, ":/texture/link135-white.png", "wlink135" },
  { Icon::GLink,  Icon::WLink180, ":/texture/link180-white.png", "wlink180" },
  { Icon::GLink,  Icon::WLink225, ":/texture/link225-white.png", "wlink225" },
  { Icon::GLink,  Icon::WLink270, ":/texture/link270-white.png", "wlink270" },
  { Icon::GLink,  Icon::WLink315, ":/texture/link315-white.png", "wlink315" },
  { Icon::GMedia, Icon::WMedia,   ":/texture/media-white.png",   "wmedia" },
  { Icon::GMusic, Icon::WMusic,   ":/texture/music-white.png",   "wmusic" },
  { Icon::GInfo,  Icon::BInfo,    ":/texture/info-black.png",    "binfo" },
  { Icon::GExit,  Icon::BExit,    ":/texture/door-black.png",    "bexit" },
  { Icon::GLink,  Icon::BLink000, ":/texture/link000-black.png", "blink000" },
  { Icon::GLink,  Icon::BLink045, ":/texture/link045-black.png", "blink045" },
  { Icon::GLink,  Icon::BLink090, ":/texture/link090-black.png", "blink090" },
  { Icon::GLink,  Icon::BLink135, ":/texture/link135-black.png", "blink135" },
  { Icon::GLink,  Icon::BLink180, ":/texture/link180-black.png", "blink180" },
  { Icon::GLink,  Icon::BLink225, ":/texture/link225-black.png", "blink225" },
  { Icon::GLink,  Icon::BLink270, ":/texture/link270-black.png", "blink270" },
  { Icon::GLink,  Icon::BLink315, ":/texture/link315-black.png", "blink315" },
  { Icon::GMedia, Icon::BMedia,   ":/texture/media-black.png",   "bmedia" },
  { Icon::GMusic, Icon::BMusic,   ":/texture/music-black.png",   "bmusic" },
} ;


const char *Icon::textureFile(IconType num) {
        if (num>sizeof(iconList)) return iconList[0].icontexture ;
        return iconList[(int)num].icontexture ;
}

const char *Icon::menuFile(IconType num) {
        if (num>sizeof(iconList)) return iconMenuTextures[0] ;
        return iconMenuTextures[(int)iconList[(int)num].group] ;
}

Icon::Group Icon::textureGroup(IconType num) {
        if (num>sizeof(iconList)) return iconList[0].group ;
        return iconList[(int)num].group ;
}

const char *Icon::name(IconType num) {
    if (num>sizeof(iconList)) return iconList[0].iconname ;
    return iconList[(int)num].iconname ;
}
