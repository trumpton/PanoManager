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
// Tour Properties Dialog
//

#include "tourpropertiesdialog.h"
#include "ui_tourpropertiesdialog.h"

TourPropertiesDialog::TourPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TourPropertiesDialog)
{
    ui->setupUi(this);
}

TourPropertiesDialog::~TourPropertiesDialog()
{
    delete ui;
}

void TourPropertiesDialog::load(Project *proj)
{
    if (!proj) return ;
    ui->author_lineEdit->setText(proj->author()) ;
    ui->autoLoad_checkBox->setChecked(proj->autoLoad()) ;
    ui->autoRotate_lineEdit->setText(QString::number(proj->autoRotate())) ;
    ui->compass_checkBox->setChecked(proj->compass()) ;
    ui->debug_checkBox->setChecked(proj->debug()) ;
    ui->overwriteLibrary_checkBox->setChecked(proj->overwriteLibrary()) ;
    ui->sceneFade_lineEdit->setText(QString::number(proj->sceneFade())) ;
    ui->firstSceneLat_lineEdit->setText(QString::number(proj->startingSceneLat()/1000)) ;
    ui->firstSceneLon_lineEdit->setText(QString::number(proj->startingSceneLon()/1000)) ;
    ui->title_LineEdit->setText(proj->title()) ;
    for (int i=0; i<proj->sceneCount(); i++) {
        ui->firstScene_comboBox->addItem(proj->sceneAt(i).title(), proj->sceneAt(i).id());
    }
    QString id = proj->startingSceneId() ;
    int idx = ui->firstScene_comboBox->findData(id, Qt::UserRole) ;
    ui->firstScene_comboBox->setCurrentIndex(idx);
}

void TourPropertiesDialog::save(Project *proj)
{
    if (!proj) return ;
    proj->setAuthor(ui->author_lineEdit->text()) ;
    proj->setAutoLoad(ui->autoLoad_checkBox->isChecked()) ;
    proj->setAutoRotate(ui->autoRotate_lineEdit->text().toInt());
    proj->setCompass(ui->compass_checkBox->isChecked()) ;
    proj->setDebug(ui->debug_checkBox->isChecked()) ;
    proj->setOverwriteLibrary(ui->overwriteLibrary_checkBox->isChecked());
    proj->setSceneFade(ui->sceneFade_lineEdit->text().toInt());
    proj->setStartingScene(
                ui->firstScene_comboBox->currentData().toString(),
                ui->firstSceneLat_lineEdit->text().toInt()*1000,
                ui->firstSceneLon_lineEdit->text().toInt()*1000) ;
    proj->setTitle(ui->title_LineEdit->text());
}
