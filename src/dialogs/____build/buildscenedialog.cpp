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

#include "buildscenedialog.h"
#include "ui_buildscenedialog.h"
#include "maptranslation.h"
#include "sceneimage.h"

#include <QFile>
#include <QFileInfo>

BuildSceneDialog::BuildSceneDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuildSceneDialog)
{
    ui->setupUi(this);
}

BuildSceneDialog::~BuildSceneDialog()
{
    delete ui;
}

bool BuildSceneDialog::BatchBuild(QStringList files)
{
    SceneImage img ;
    int n = files.size() ;
    bool success=true ;
    ui->buildingScene_progressBar->setMinimum(0) ;
    ui->buildingScene_progressBar->setMaximum(1200*n) ;
    ui->buildingScene_progressBar->setValue(0) ;
    this->show() ;
    for (int i=0; i<n; i++) {
        success = success && DoBuild(ui->buildingScene_progressBar, files.at(i), &img, i, n, false, false, true) ;
    }
    this->hide() ;
    return success ;
}

bool BuildSceneDialog::Load(QString file, SceneImage *scene, bool loadpreview)
{
    bool success ;
    this->show() ;
    ui->buildingScene_progressBar->setMinimum(0) ;
    ui->buildingScene_progressBar->setMaximum(1200) ;
    ui->buildingScene_progressBar->setValue(0) ;
    success = DoBuild(ui->buildingScene_progressBar, file, scene, 0, 1, loadpreview, true, false) ;
    this->hide() ;
    return success ;
}

bool BuildSceneDialog::DoBuild(QProgressBar *bar, QString file, SceneImage *scene, int seq, int of, bool loadpreview, bool buildpreview, bool buildonly)
{
    QFileInfo fi(file) ;

    bool success = true ;
    ProgressDialog prog ;
    prog.setTitle("Building") ;
    prog.setText1("scene " + QString::number(seq)) ;
    prog.show() ;
    prog.setMaximum(300);
    success = scene->loadImage(&prog, file, loadpreview, buildpreview, buildonly) ;
    prog.hide() ;
    return success ;
}


