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
// PanoManager Main Function
//

#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QFont>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool useNativeFileDialog = false ;
    bool useSystemFonts = true ;

    int c ;
    while ((c = getopt(argc, argv, "hnNfF")) != -1) {
        switch (c) {
            case 'n':
                useNativeFileDialog=false ;
                break ;
            case 'N':
                useNativeFileDialog=true ;
                break ;
            case 'f':
                useSystemFonts=false ;
                break ;
            case 'F':
                useSystemFonts=true ;
                break ;
            case 'h':
            case '?':
                printf("panomanager [-h] [-n|N] [-f|F]\n") ;
                printf(" -N       Use Native File Dialog (default)\n") ;
                printf(" -n       Use System File Dialog\n") ;
                printf(" -f       Use in-built Fonts\n") ;
                printf(" -F       Use System Fonts (default)\n") ;
                break ;
        }
    }

    if (!useSystemFonts) {
        int fontId = QFontDatabase::addApplicationFont(":/fonts/DejaVuSans.ttf");
        if (fontId != -1) {
            QFont font("DejaVuSans");
            a.setFont(font) ;
        }
    }

    MainWindow w;
    w.setOptions(useNativeFileDialog) ;
    w.show();

    return a.exec();
}
