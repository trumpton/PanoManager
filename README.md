# PanoManager - Panorama Manager

This is a tool, built with QT that is used to manage panorama tours.
It loads equirectangular images, creates cube faces (which can then be annotated), and allows the interactive generation of tours. See the user manual, which can be found at http://www.trumpton.uk/panomanager. 

## Overview

A Tour contains:
* Scenes - A scene contains six cube-face images, and a number of nodes.
* Nodes - These can be links to other scenes, information points, or links to media files.

A tour is saved in a 'pmp' file.  Once complete, the tour can be exported to a folder, where the appropriate web files are created.

Tours can currently be created for 'Pannellum' and 'Marzipano', and a snapshot of the Pannellum and Marzipano built files are included (with links to get the latest versions directly).

Once exported, Pano Manager can launch a third-party web server (e.g. using php) so that the output can be viewed on a local disk, but using a web browser.

## Use

panomanager [ -h ] [ -f|F ] [ -n|N ]

  -n|N  -  Selects which file dialogs to use: -n = Qt (default), -N = System.
  -f|F  -  Selects which fonts to use: -f = Built-in DejaVu, -F = System (default).


## Licencing

The following Licences apply:

* PanoManager: GPL
* QT: GPL / LGPL
* Pannellum: MIT
* Marzipano: Apache
* DejaVu: Copyright

## Limitations and Caveats

The PanoManager program is provided without warranty.  I am not a software engineer, and don't provide any warranty as to the quality, performance or maintainability of this code.  Please feel free to copy and expand the program, but ensure that all authors are appropriately recognised.

## Thankyou

If you want to say thankyou for the program, please consider making a donation to Guide Dogs for the Blind here: https://www.justgiving.com/fundraising/panomanager18

## Author

PanoManager - (c) Steve Clarke 2018

