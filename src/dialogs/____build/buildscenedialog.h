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
// Build Scene Dialog
//

#ifndef BUILDSCENEDIALOG_H
#define BUILDSCENEDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QProgressBar>
#include "sceneimage.h"

namespace Ui {
class BuildSceneDialog;
}

class BuildSceneDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BuildSceneDialog(QWidget *parent = 0);
    ~BuildSceneDialog();
    bool BatchBuild(QStringList files) ;
    bool Load(QString file, SceneImage *scene, bool loadpreview) ;

private:
    Ui::BuildSceneDialog *ui;
    bool DoBuild(QProgressBar *prog, QString file, SceneImage *scene, int seq, int of, bool loadpreview, bool buildpreview, bool buildonly) ;
};

#endif // BUILDSCENEDIALOG_H
