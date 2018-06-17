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


/*#ifdef __MINGW32__
#include <unistd.h>
#else
#include <io.h>
#endif
*/

int getopt(int nargc, char * const nargv[], const char *ostr) ;

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


#include <string.h>
#include <stdio.h>

int  opterr = 1,             /* if error message should be printed */
     optind = 1,             /* index into parent argv vector */
     optopt,                 /* character checked for validity */
     optreset;               /* reset getopt */
char *optarg;                /* argument associated with option */

#define BADCH   (int)'?'
#define BADARG  (int)':'
#define EMSG    (char *)""

/*
* getopt --
*      Parse argc/argv argument vector.
*/
int
  getopt(int nargc, char * const nargv[], const char *ostr)
{
  static char *place = EMSG;              /* option letter processing */
  const char *oli;                        /* option letter list index */

  if (optreset || !*place) {              /* update scanning pointer */
    optreset = 0;
    if (optind >= nargc || *(place = nargv[optind]) != '-') {
      place = EMSG;
      return (-1);
    }
    if (place[1] && *++place == '-') {      /* found "--" */
      ++optind;
      place = EMSG;
      return (-1);
    }
  }                                       /* option letter okay? */
  if ((optopt = (int)*place++) == (int)':' ||
    !(oli = strchr(ostr, optopt))) {
      /*
      * if the user didn't specify '-' as an option,
      * assume it means -1.
      */
      if (optopt == (int)'-')
        return (-1);
      if (!*place)
        ++optind;
      if (opterr && *ostr != ':')
        (void)printf("illegal option -- %c\n", optopt);
      return (BADCH);
  }
  if (*++oli != ':') {                    /* don't need argument */
    optarg = NULL;
    if (!*place)
      ++optind;
  }
  else {                                  /* need an argument */
    if (*place)                     /* no white space */
      optarg = place;
    else if (nargc <= ++optind) {   /* no arg */
      place = EMSG;
      if (*ostr == ':')
        return (BADARG);
      if (opterr)
        (void)printf("option requires an argument -- %c\n", optopt);
      return (BADCH);
    }
    else                            /* white space */
      optarg = nargv[optind];
    place = EMSG;
    ++optind;
  }
  return (optopt);                        /* dump back option letter */
}
