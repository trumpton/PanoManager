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
// PanoManager Main Window
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QListWidgetItem>

#include "project/project.h"
#include "sceneimage/sceneimage.h"
#include "dialogs/progress/progressdialog.h"
#include "dialogs/webserver/webserver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QListWidgetItem* findListWidgetItemById(QListWidget *widget, QString id) ;

public slots:
    void on_realBearingChanged(int lat, int lon) ;

private slots:
    void on_action_Open_Project_triggered();
    void on_action_Save_Project_triggered();
    void on_action_Save_Project_As_triggered();
    void on_action_New_Project_triggered();
    void on_addscene_pushButton_clicked();
    void on_action_Build_Hi_Res_Scenes_triggered();
    void on_nodeSet_pushButton_clicked();
    void on_nodeAdd_pushButton_clicked();
    void on_setNorth_pushButton_clicked();
    void on_nodetype_comboBox_currentIndexChanged(int index);
    void on_nodedestination_comboBox_currentIndexChanged(int index);
    void on_nodedescription_plainTextEdit_textChanged();
    void on_nodetitle_lineEdit_textChanged(const QString &arg1);
    void on_turnAround_pushButton_clicked();
    void on_nodeDelete_pushButton_clicked();
    void on_deletescene_pushButton_clicked();
    void on_node_listWidget_itemClicked(QListWidgetItem *item);
    void on_scenes_listWidget_itemClicked(QListWidgetItem *item);
    void on_nodeGo_pushButton_clicked();
    void on_node_arrivalLat_lineEdit_textEdited(const QString &arg1);
    void on_node_arrivalLon_lineEdit_textEdited(const QString &arg1);
    void on_sceneTitle_lineEdit_textChanged(const QString &arg1);
    void on_action_ExportPanellum_triggered();
    void on_action_ExportMarzipano_triggered();
    void on_action_Properties_triggered();
    void on_nodeUrl_lineEdit_textEdited(const QString &arg1);
    void on_action_Licenses_triggered();

    void on_action_Web_Server_triggered();

    void on_actionE_xit_triggered();

private:
    WebServer webserver ;
    SceneImage sceneimage ;
    Project project ;
    QSettings *settings;
    Ui::MainWindow *ui;
    QString m_currentScene, m_lastSceneScene ;
    QString m_currentNode, m_lastNode, m_lastNodeScene ;
    void changeScene(QString id) ;
    void refreshScenes(QString selectedScene) ;
    void refreshNodes(QString selectedNode) ;
    void buildExportTiles(QString outputFolder, QString mask) ;
    bool checkProject(QString dir) ;
    bool exportFaces(ProgressDialog *progress, Scene scene, int tilesize, const char *masks[], QString folder, int *levels, int *cuberesolution) ;
    PM::Err DoBuild(ProgressDialog *progress, QString file, SceneImage *scene, int seq, int of, bool loadpreview, bool buildpreview, bool buildonly) ;
    void exportPanellumFiles(QString folder, QString title);
    void exportMarzipanoFiles(QString folder, QString title);

    void copyResourceFolder(QString foldersrc, QString dest, bool forceOverwrite) ;
    void copyFile(QString source, QString dest, bool forceOverwrite) ;


};

#endif // MAINWINDOW_H
