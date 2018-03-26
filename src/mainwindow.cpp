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

#include "version.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QFileInfo>
#include <QBitmap>
#include <QIcon>
#include <QMatrix>
#include <QDebug>
#include <QtCore/qmath.h>

#include "sceneimage/sceneimage.h"
#include "icons/icons.h"
#include "dialogs/progress/progressdialog.h"
#include "errors/pmerrors.h"
#include "dialogs/tourproperties/tourpropertiesdialog.h"
#include "dialogs/about/aboutdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_currentScene="" ;
    m_currentNode="" ;

    ui->setupUi(this);
    this->setWindowTitle("Pano Manager") ;
    this->setAccessibleName("Pano Manager") ;

    settings = new QSettings("trumpton.org.uk", "panomanager") ;

    // Populate Node Types Menu
    for (unsigned int i=0; i<Icon::numTextures; i++) {
        QMatrix rot ;
        QPixmap pm ;
        QImage img(Icon::textureFile((Icon::IconType)i));
        QIcon icon(pm.fromImage(img.transformed(rot))) ;
        ui->nodetype_comboBox->addItem(icon, QString(""), (int)i) ;
    }
    // Override background colour (inc dropped-down) to aid readability
    ui->nodetype_comboBox->setStyleSheet(
                "QComboBox, QComboBox QAbstractItemView { background-color: #8888AA; } " \
                "QComboBox[enabled=false] { background-color: #EEEEEE; }");

    // Setup the web server
    webserver.setCommand(settings->value("webservercommand","php -S localhost:8000").toString()) ;
    webserver.setServerFolder(settings->value("webserverfolder", "").toString());

    // Clear out, and put app in a known fixed state
    on_action_New_Project_triggered();

    // Signal and Slot Connection for live update of bearing
    connect(ui->display, SIGNAL(realBearingChanged(int,int)), this, SLOT(on_realBearingChanged(int,int)));
}

MainWindow::~MainWindow()
{
    ui->display->clearScene() ;

    // Refresh to free allocated memory
    refreshScenes("") ;
    refreshNodes("") ;
    delete ui;
}

void MainWindow::setOptions(bool useNativeFileDialog)
{
    if (!useNativeFileDialog) {
        m_fdOptions = QFileDialog::DontUseNativeDialog ;
    } else {
        m_fdOptions = (QFileDialog::Option)0 ;
    }
}

//======================================================================================================================
//
// Form Refresh Functions
//
// refreshScenes            - Update the scenes form
// refreshNodes             - Update the nodes and enable the appropriate parts of the form
// findListWidgetItemById   - Search the list widget, and return pointer to item which has matching id, or NULL
//

//----------------------------------------------------------------------------------------------------------------------
//
// refreshScenes
//

void MainWindow::refreshScenes(QString selectedScene)
{

    static int mutex=0 ;
    if (mutex>0) {
        // Deleting listwidgetitem removes it from the list
        // It also de-selects it if it was selected, resulting in a
        // QListWidget_selection_changed message, and an attempted
        // call to refresScenes(), which is ignored by the mutex
        qDebug() << "refreshScenes() re-entrant call - exited" ;
        return ;
    }
    mutex++ ;

    qDebug() << "refreshScenes(" << selectedScene << ") started" ;

    for (int i=0; i<project.sceneCount(); i++) {
        Scene& scene = project.sceneAt(i) ;
        QString scenetitle = scene.title() ;
        QString sceneId = scene.id() ;
        QListWidgetItem *item = findListWidgetItemById(ui->scenes_listWidget, sceneId) ;
        if (item) {
            // Update Entry
            item->setText(scenetitle) ;
            qDebug() << " --- scene update:  " << sceneId ;
        } else {
            // Add New Entry
            item = new QListWidgetItem;
            item->setText(scenetitle) ;
            item->setData(Qt::UserRole, sceneId) ;
            ui->scenes_listWidget->addItem(item) ;
            qDebug() << " --- scene add:     " << sceneId ;
        }
    }

    // Remove invalid scene entries (note findItems takes a COPY of the list)
    // so it is safe to iterate through this list despite deleteing actual items.
    QList<QListWidgetItem*> items = ui->scenes_listWidget->findItems("", Qt::MatchContains) ;
    foreach (QListWidgetItem *item, items) {
        QString sceneId = item->data(Qt::UserRole).toString() ;
        Scene& scene = project.scene(sceneId) ;
        if (!scene.isValid()) {
            // delete item removes it from the list
            QListWidgetItem *lwi = findListWidgetItemById(ui->scenes_listWidget, sceneId) ;
            if (lwi) delete lwi ;
            qDebug() << " --- scene remove: " << sceneId ;
        }
    }

    if (project.scene(selectedScene).isValid()) {
        // Update current scene to match selected, and highlight it as selected
        m_currentScene=selectedScene ;
        QListWidgetItem *item = findListWidgetItemById(ui->scenes_listWidget, m_currentScene) ;
        if (item && ui->scenes_listWidget->currentItem()!=item) {
            ui->scenes_listWidget->clearSelection() ;
            ui->scenes_listWidget->setCurrentItem(item, QItemSelectionModel::Select) ;
        }
        ui->sceneTitle_lineEdit->setText(project.scene(m_currentScene).title()) ;
    } else {
        // Unset the current item
        ui->scenes_listWidget->setCurrentRow(-1) ;
        ui->scenes_listWidget->clearSelection() ;
        ui->sceneTitle_lineEdit->setText("") ;
        m_currentScene="" ;
    }

    // Refresh Node Destinations
    int nodeDest = ui->nodedestination_comboBox->currentIndex() ;
    ui->nodedestination_comboBox->clear() ;
    for (int i=0; i<project.scenes().count(); i++) {
        Scene& scene = project.sceneAt(i) ;
        QString scenetitle = scene.title() ;
        QString sceneid = scene.id() ;
        ui->nodedestination_comboBox->addItem(scenetitle, sceneid);
    }
    ui->nodedestination_comboBox->setCurrentIndex(nodeDest);

    bool showSceneDetails = !m_currentScene.isEmpty() ;
    ui->scenedetails_groupBox->setEnabled(showSceneDetails) ;
    ui->action_Add_Scene->setEnabled(true) ;

    bool someScenes = ui->scenes_listWidget->selectedItems().count()>0 ;
    ui->action_Delete_Scene->setEnabled(someScenes) ;
    ui->deletescene_pushButton->setEnabled(someScenes) ;

    qDebug() << "refreshScenes(" << selectedScene << ") completed" ;

    mutex-- ;

}

//----------------------------------------------------------------------------------------------------------------------
//
// refreshNodes
//

void MainWindow::refreshNodes(QString selectedNode)
{
    static int mutex=0 ;
    if (mutex>0) {
        // Deleting listwidgetitem removes it from the list
        // It also de-selects it if it was selected, resulting in a
        // QListWidget_selection_changed message, and an attempted
        // call to refreshNodes(), which is ignored by the mutex
        qDebug() << "refreshNodes() re-entrant call - exited" ;
        return ;
    }
    mutex++ ;

    qDebug() << "refreshNodes(" << selectedNode << ") started" ;

    Scene& scene = project.scene(m_currentScene) ;

    // Update Existing Entries and Add New
    for (int j=0; j<scene.nodeCount(); j++) {
        Node& node = scene.nodeAt(j) ;
        QString nodeId = scene.nodeAt(j).id() ;
        QIcon icon(Icon::menuFile(node.type())) ;
        QListWidgetItem *item = findListWidgetItemById(ui->node_listWidget, nodeId) ;
        if (item) {
            // Update Entry
            item->setText(node.title());
            item->setIcon(icon) ;
            qDebug() << " --- node update:  " << nodeId ;
        } else {
            // Add New Entry
            item = new QListWidgetItem ;
            item->setText(node.title());
            item->setData(Qt::UserRole, nodeId) ;
            item->setIcon(icon) ;
            ui->node_listWidget->addItem(item) ;
            qDebug() << " --- node add:     " << nodeId ;
        }
    }

    // Remove invalid node entries (note findItems takes a COPY of the list)
    QList<QListWidgetItem*> items = ui->node_listWidget->findItems("", Qt::MatchContains) ;
    foreach (QListWidgetItem *item, items) {
        QString nodeId = item->data(Qt::UserRole).toString() ;
        Node& node = scene.node(nodeId) ;
        if (!node.isValid()) {
            QListWidgetItem *lwi = findListWidgetItemById(ui->node_listWidget, nodeId) ;
            if (lwi) delete lwi ;
            qDebug() << " --- node remove: " << nodeId ;
        }
    }

    if (project.scene(m_currentScene).node(selectedNode).isValid()) {

        m_currentNode = selectedNode ;

        // Node has changed
        Node& node = scene.node(m_currentNode) ;

        // Force item to be selected
        QListWidgetItem *item = findListWidgetItemById(ui->node_listWidget, m_currentNode) ;
        ui->node_listWidget->clearSelection() ;
        if (item) {
            ui->node_listWidget->setCurrentItem(item, QItemSelectionModel::Select) ;
        }

        // Extract parameters for Node
        int bearing = node.arrivalLon() ;
        int elev = node.arrivalLat() ;
        int type = node.type() ;
        QString title = node.title() ;
        QString desc = node.description() ;
        QString destsceneref = node.destId() ;
        QString url = node.url() ;

        // Select node destination index
        if (ui->nodedestination_comboBox->currentData().toString().compare(destsceneref)!=0) {
            int row = ui->nodedestination_comboBox->findData(destsceneref, Qt::UserRole) ;
            ui->nodedestination_comboBox->setCurrentIndex(row);
        }

        // icon type index
        if (ui->nodetype_comboBox->currentIndex()!=type) {
            ui->nodetype_comboBox->setCurrentIndex(type);
        }

        // other node parameters
        ui->node_arrivalLon_lineEdit->setText(QString::number(bearing/1000.0f)) ;
        ui->node_arrivalLat_lineEdit->setText(QString::number(elev/1000.0f)) ;
        ui->nodetitle_lineEdit->setText(title) ;
        ui->nodedescription_plainTextEdit->document()->setPlainText(desc) ;
        ui->nodeUrl_lineEdit->setText(url) ;

    } else {

        // Clear Details
        ui->nodetype_comboBox->setCurrentIndex(0);
        ui->node_arrivalLon_lineEdit->setText("0") ;
        ui->node_arrivalLat_lineEdit->setText("0") ;
        ui->nodetitle_lineEdit->clear() ;
        ui->nodedescription_plainTextEdit->document()->clear() ;
        ui->nodeUrl_lineEdit->clear() ;
        ui->nodedestination_comboBox->setCurrentIndex(-1) ;

        // Unset the current item
        ui->node_listWidget->setCurrentRow(-1) ;
        ui->node_listWidget->clearSelection() ;
        m_currentNode="" ;
    }

    bool showNodes = !m_currentScene.isEmpty() ;
    ui->nodelist_groupBox->setEnabled(showNodes) ;
    ui->nodedetails_groupBox->setEnabled(showNodes) ;
    ui->action_Add_Node->setEnabled(showNodes) ;
    ui->nodeAdd_pushButton->setEnabled(showNodes) ;

    bool someNodes = ui->node_listWidget->selectedItems().count()>0 ;
    ui->action_Delete_Node->setEnabled(someNodes) ;
    ui->nodeDelete_pushButton->setEnabled(someNodes) ;

    bool someScenes = ui->scenes_listWidget->selectedItems().count()>0 ;
    ui->action_Delete_Scene->setEnabled(someScenes) ;
    ui->action_Delete_Scene->setEnabled(someScenes) ;

   ui->display->setSelectedNode(m_currentNode);
   ui->display->refresh();
   qDebug() << "refreshNode(" << selectedNode << ") complete" ;

   mutex-- ;
}

//----------------------------------------------------------------------------------------------------------------------
//
// findListWidgetItemById
//

QListWidgetItem* MainWindow::findListWidgetItemById(QListWidget *widget, QString id)
{
    if (widget) {
        int count = widget->count() ;
        for (int i=0; i<count; i++) {
            if (widget->item(i)->data(Qt::UserRole).toString().compare(id)==0) return widget->item(i) ;
        }
    }
    return NULL ;
}


//======================================================================================================================
//
// Scene List Management Handlers
//
//  on_addscene_pushButton_clicked          -   Add a new Scene
//  on_deletescene_pushButton_clicked       -   Delete the current scene
//  on_scenes_listWidget_itemClicked        -   User selected different scene
//

//----------------------------------------------------------------------------------------------------------------------
//
// on_addscene_pushButton_clicked
//

void MainWindow::on_addscene_pushButton_clicked()
{
    qDebug() << "Add_Scene()" ;

    QString lastimagedir = settings->value("lastimagedir", "").toString() ;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import Rectilinear Image"), lastimagedir, tr("Jpeg Image (*.jpg); Tif Image (*.tiff); PNG Image (*.png); All Files (*.*)"), Q_NULLPTR, m_fdOptions);
    if (!fileName.isEmpty()) {
        QFileInfo path(fileName) ;
        settings->setValue("lastimagedir", path.absoluteFilePath()) ;
        Scene newScene ;
        newScene.setFilename(fileName) ;
        newScene.setTitle(path.baseName()) ;
        newScene.setNorthOffset(0) ;
        project.scenes().append(newScene) ;
        changeScene(newScene.id()) ;
    }
    qDebug() << "Add_Scene() complete" ;
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_deletescene_pushButton_clicked
//

void MainWindow::on_deletescene_pushButton_clicked()
{
    qDebug() << "Delete_Scene()" ;

    if (project.scene(m_currentScene).isValid()) {
        // Remove the scene from the display, then the scenes list, then refresh
        ui->display->clearScene() ;
        project.removeScene(m_currentScene) ;
        refreshScenes("") ;
        refreshNodes("") ;
    }


    qDebug() << "Delete_Scene() complete" ;
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_scenes_listWidget_itemClicked
//

void MainWindow::on_scenes_listWidget_itemClicked(QListWidgetItem *item)
{
    QString newsceneId ;

    if (item) {
        newsceneId = item->data(Qt::UserRole).toString() ;
    }

    if (m_currentScene.compare(newsceneId)==0) return ;

    qDebug() << "scenes_listWidget_itemClicked(" << newsceneId << ")" ;

    changeScene(newsceneId) ;
    ui->display->setCamera(0, project.scene(m_currentScene).northOffset()) ;

    qDebug() << "scenes_listWidget_itemClicked() complete" ;

}

void MainWindow::handleChangeScenePercentUpdate(int percent)
{
    m_prog.setValue(percent) ;
}

void MainWindow::changeScene(QString id)
{
    int north ;
    QString title ;
    PM::Err err = PM::Ok ;

    Scene& selectedScene = project.scene(id) ;

    if (project.scene(selectedScene.id()).isValid()) {

        // Build and Load new scene       
        bool loadhires = ui->action_Load_Hi_Res_If_Avail->isChecked() ;

        m_prog.setTitle("Changing Scene") ;
        m_prog.setText1("Loading Scene: " + selectedScene.title()) ;
        m_prog.setMaximum(100);
        m_prog.show() ;

        connect(&sceneimage, SIGNAL(percentUpdate(int)), this, SLOT(handleChangeScenePercentUpdate(int))) ;
        err=DoBuild(selectedScene.filename(), &sceneimage, 0, 1, !loadhires, true, true, false) ;
        disconnect(&sceneimage, SIGNAL(percentUpdate(int)), this, SLOT(handleChangeScenePercentUpdate(int))) ;

        if (err==PM::Ok) {

            // Load Scene
            ui->display->loadScene(&selectedScene.nodes(), sceneimage.getFace(0), sceneimage.getFace(1), sceneimage.getFace(2), sceneimage.getFace(3), sceneimage.getFace(4), sceneimage.getFace(5)) ;

            // Update scene title & North
            north = selectedScene.northOffset() ;
            title = selectedScene.title() ;

        } else {

            QMessageBox::critical(nullptr, QString("Error Changing Scene"), PM::errString(err)) ;

            // Clear Scene
            ui->display->clearScene() ;

            // Update scene title & North
            north=0 ;
            title = "" ;

        }

        // Update scene and node list
        refreshScenes(id) ;
        ui->display->setNorthCompassLon(north);
        refreshNodes("") ;

        m_prog.hide() ;

    }


}

//======================================================================================================================
//
// Scene Editing
//
//  on_setNorth_pushButton_clicked         -   Set scene's North Bearing
//  on_sceneTitle_lineEdit_editingFinished -   Save title, and refresh
//


//----------------------------------------------------------------------------------------------------------------------
//
// on_setNorth_pushButton_clicked
//

void MainWindow::on_setNorth_pushButton_clicked()
{
    qDebug() << "Set_North(" << ui->display->lon() << ", " << ui->display->lat() << ")" ;

    if (m_currentScene<0) return ;
    int north = ui->display->lon() ;
    project.scene(m_currentScene).setNorthOffset(north) ;
    ui->display->setNorthCompassLon(north) ;
}


//----------------------------------------------------------------------------------------------------------------------
//
// on_sceneTitle_lineEdit_editingFinished
//
void MainWindow::on_sceneTitle_lineEdit_editingFinished()
{
    Scene& scene = project.scene(m_currentScene) ;
    QString arg1 = ui->sceneTitle_lineEdit->text() ;
    if (scene.title().compare(arg1)!=0) {
        scene.setTitle(arg1) ;
        refreshScenes(m_currentScene) ;
    }

}

//======================================================================================================================
//
// Movement
//
// on_realBearingChanged            - Callback from display, updates when user moves the camera view
// on_turnAround_pushButton_clicked - Turn user view around 180 degrees
//

//----------------------------------------------------------------------------------------------------------------------
//
// on_realBearingChanged
//

void MainWindow::on_realBearingChanged(int lat, int lon)
{
    ui->currentLon_label->setText(QString::number(lon/1000)) ;
    ui->currentLat_label->setText(QString::number(lat/1000)) ;
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_turnAround_pushButton_clicked
//

void MainWindow::on_turnAround_pushButton_clicked()
{
    int lat = ui->display->lat() ;
    int lon = ui->display->lon() ;
    lat = -lat ;
    lon+=180000 ;
    if (lon>=360000) lon-=360000 ;
    ui->display->setCamera(lat, lon) ;
}


//======================================================================================================================
//
// Node Selection and Management Handlers
//
// on_nodeAdd_pushButton_clicked      - Add a new node
// on_node_listWidget_itemClicked     - Change selected node
// on_nodeDelete_pushButton_clicked   - Remove a node
//


//----------------------------------------------------------------------------------------------------------------------
//
// on_nodeAdd_pushButton_clicked
//

void MainWindow::on_nodeAdd_pushButton_clicked()
{
    qDebug() << "Node_Add(" << ui->display->lon() << ", " << ui->display->lat() << ")" ;
    Scene& scene = project.scene(m_currentScene) ;
    if (!scene.isValid()) return ;
    Node node ;
    node.setLat(ui->display->lat()) ;
    node.setLon(ui->display->lon()) ;
    scene.nodes().append(node) ;
    refreshNodes(node.id()) ;
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_nodeDelete_pushButton_clicked
//
void MainWindow::on_nodeDelete_pushButton_clicked()
{
    qDebug() << "Node_Delete(" << m_currentNode << ")" ;
    Node& node = project.scene(m_currentScene).node(m_currentNode) ;
    if (node.isValid()) {
        project.scene(m_currentScene).removeNode(m_currentNode) ;
    }
    refreshNodes("") ;

    qDebug() << "Node_Delete() complete" ;
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_node_listWidget_itemClicked
//

void MainWindow::on_node_listWidget_itemClicked(QListWidgetItem *item)
{
    QString newNodeId = item->data(Qt::UserRole).toString() ;
    qDebug() << "node_listWidget_itemClicked(" << newNodeId << ")" ;
    refreshNodes(newNodeId) ;
    int lat = project.scene(m_currentScene).node(m_currentNode).lat() ;
    int lon = project.scene(m_currentScene).node(m_currentNode).lon() ;
    ui->display->setCamera(lat, lon) ;
    qDebug() << "node_listWidget_currentItemChanged() complete" ;
}

//======================================================================================================================
//
// Node Editing Handlers
//
// on_nodeGo_pushButton_clicked                     - Change scene (follow link)
// on_nodedestination_comboBox_currentIndexChanged  - User has selected a different destination for a link
// on_node_arrivaLlat_lineEdit_textEdited           - User has updated the node's arrival latitude
// on_node_arrivaLlon_lineEdit_textEdited           - User has updated the node's arrival longitude
// on_nodetitle_lineEdit_textChanged                - User has updated the node's title
// on_nodedescription_plainTextEdit_textChanged     - User has updated the node's description
// on_nodeSet_pushButton_clicked                    - User has selected a new target lat/lon for the node icon
// on_nodetype_comboBox_currentIndexChanged         - User has change selected node
//

//----------------------------------------------------------------------------------------------------------------------
//
// on_nodeGo_pushButton_clicked
//

void MainWindow::on_nodeGo_pushButton_clicked()
{
    Node& node = project.scene(m_currentScene).node(m_currentNode) ;
    Scene& dest = project.scene(node.destId()) ;
    if (dest.isValid()) {
        changeScene(dest.id()) ;
        int lon = node.arrivalLon() + dest.northOffset() ;
        while (lon>360000) lon-=360000 ;
        int lat = 0 ;
        ui->display->setCamera(lat, lon) ;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_nodedestination_comboBox_currentIndexChanged
//

void MainWindow::on_nodedestination_comboBox_currentIndexChanged(int index)
{
    Node& node = project.scene(m_currentScene).node(m_currentNode) ;
    if (node.destId().compare(project.sceneAt(index).id())!=0) {
        node.setDestId(project.sceneAt(index).id()) ;
        refreshNodes(m_currentNode) ;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_node_arrivalLat_lineEdit_editingFinished
//
void MainWindow::on_node_arrivalLat_lineEdit_editingFinished()
{
    Node& node = project.scene(m_currentScene).node(m_currentNode) ;
    QString arg1 = ui->node_arrivalLat_lineEdit->text() ;
    int bearing = arg1.toDouble() * 1000 ;
    if (node.arrivalLat()!=bearing) {
        node.setArrivalLat(bearing) ;
        refreshNodes(m_currentNode) ;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_node_arrivalLon_lineEdit_editingFinished
//
void MainWindow::on_node_arrivalLon_lineEdit_editingFinished()
{
    Node& node = project.scene(m_currentScene).node(m_currentNode) ;
    QString arg1 = ui->node_arrivalLon_lineEdit->text() ;
    int bearing = arg1.toDouble() * 1000 ;
    if (node.arrivalLon()!=bearing) {
        node.setArrivalLon(bearing) ;
        refreshNodes(m_currentNode) ;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_nodetitle_lineEdit_editingFinished
//
void MainWindow::on_nodetitle_lineEdit_editingFinished()
{
    Node& node = project.scene(m_currentScene).node(m_currentNode) ;
    QString text = ui->nodetitle_lineEdit->text() ;
    if (node.title().compare(text)!=0) {
        node.setTitle(text); ;
        refreshNodes(m_currentNode) ;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_nodedescription_plainTextEdit_textChanged
//

void MainWindow::on_nodedescription_plainTextEdit_textChanged()
{
    Node& node = project.scene(m_currentScene).node(m_currentNode) ;
    QString text = ui->nodedescription_plainTextEdit->document()->toPlainText() ;
    if (node.description().compare(text)!=0) {
        node.setDescription(text); ;
        refreshNodes(m_currentNode) ;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_nodeUrl_lineEdit_editingFinished
//
void MainWindow::on_nodeUrl_lineEdit_editingFinished()
{
    Node& node = project.scene(m_currentScene).node(m_currentNode) ;
    QString arg1 = ui->nodeUrl_lineEdit->text() ;
    if (node.url().compare(arg1)!=0) {
        node.setUrl(arg1) ;
        refreshNodes(m_currentNode) ;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_nodeSet_pushButton_clicked
//

void MainWindow::on_nodeSet_pushButton_clicked()
{
    qDebug() << "Node_Set(" << ui->display->lon() << ", " << ui->display->lat() << ")" ;

    Node& node = project.scene(m_currentScene).node(m_currentNode) ;
    node.setLat(ui->display->lat()) ;
    node.setLon(ui->display->lon()) ;
    refreshNodes(m_currentNode) ;
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_nodetype_comboBox_currentIndexChanged
//
void MainWindow::on_nodetype_comboBox_currentIndexChanged(int index)
{
    Node& node = project.scene(m_currentScene).node(m_currentNode) ;
    if (node.type() != index) {
        node.setType((Icon::IconType)index) ;
        if (node.title().isEmpty()) node.setTitle(ui->nodedestination_comboBox->currentText()) ;
        refreshNodes(m_currentNode) ;
    }
}

//======================================================================================================================
//
// Menu: File
//
//  on_action_Open_Project_triggered    -   Open New Project File
//  on_action_Save_Project_triggered    -   Save Current Project
//  on_action_Save_Project_As_triggered -   Save Current Project As
//  on_action_New_Project_triggered     -   Create a New Project
//

//----------------------------------------------------------------------------------------------------------------------
//
// on_action_Open_Project_triggered
//

void MainWindow::on_action_Open_Project_triggered()
{
    qDebug() << "Open_Project()" ;

    // Clear out, and put app in a known fixed state
    on_action_New_Project_triggered();

    QString lastdir = settings->value("lastdir", "").toString() ;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), lastdir, tr("Pano Manager Project Files (*pmp)"), Q_NULLPTR, m_fdOptions);
    if (!fileName.isEmpty()) {
        QFileInfo path(fileName) ;
        project.clear() ;
        ui->display->clearScene() ;
        settings->setValue("lastdir", path.absoluteFilePath()) ;
        settings->setValue("lastfilename", fileName) ;
        project.OpenProject(fileName) ;
        ui->scenes_groupBox->setEnabled(true) ;
        ui->display->setCamera(0, 0) ;
        ui->display->setNorthCompassLon(0);
        refreshScenes("") ;
        refreshNodes("") ;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_action_Save_Project_triggered
//

void MainWindow::on_action_Save_Project_triggered()
{
    qDebug() << "Save_Project()" ;

    if (project.isDirty()) {
        QString fileName = settings->value("lastfilename", "").toString() ;
        if (fileName.isEmpty()) {
            on_action_Save_Project_As_triggered();
        } else {
            project.SaveProject(fileName) ;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_action_Save_Project_As_triggered
//

void MainWindow::on_action_Save_Project_As_triggered()
{
    qDebug() << "Save_Project_As()" ;

    if (project.isDirty()) {
        QString lastdir = settings->value("lastdir", "").toString() ;
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Project"), lastdir, tr("Pano Manager Project Files (*pmp)"), Q_NULLPTR, m_fdOptions) ;
        if (!fileName.isEmpty()) {
            QFileInfo path(fileName) ;
            settings->setValue("lastdir", path.absoluteFilePath()) ;
            settings->setValue("lastfilename", fileName) ;
            on_action_Save_Project_triggered();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// on_action_New_Project_triggered
//

void MainWindow::on_action_New_Project_triggered()
{
    qDebug() << "New_Project()" ;

    if (project.isDirty()) {
        on_action_Save_Project_triggered() ;
    }

    ui->display->clearScene() ;
    project.clear() ;

    ui->addscene_pushButton->setFocus() ;

    settings->setValue("lastfilename", "") ;
    ui->display->setCamera(0, 0) ;
    ui->display->setNorthCompassLon(0);
    refreshScenes("") ;
    refreshNodes("") ;

}

//======================================================================================================================
//
// Menu: Scenes
//
//  on_action_Build_Hi_Res_Scenes_triggered -   Build all high-res versions of the scenes
//

//----------------------------------------------------------------------------------------------------------------------
//
// on_action_Build_Hi_Res_Scenes_triggered
//

void MainWindow::handleBuildHiresPercentUpdate(int percent)
{
    m_prog.setValue(m_buildHiresPos+percent) ;
}

void MainWindow::on_action_Build_Hi_Res_Scenes_triggered()
{
    qDebug() << "Build_Hi_Res_Scenes()" ;

    QList<int> scenesList ;

    int numscenes = project.scenes().count() ;
    for (int i=0; i<numscenes; i++) {
        scenesList.append(i) ;
    }

    SceneImage img ;
    int n = scenesList.size() ;
    m_prog.setTitle("Batch Scene Build") ;
    m_prog.setMaximum(n*100);
    m_prog.show() ;

    PM::Err err = PM::Ok ;

    m_buildHiresPos=0 ;
    for (int i=0; err==PM::Ok && i<n; i++) {
        int idx = scenesList.at(i) ;
        Scene& thisScene = project.sceneAt(idx) ;
        m_prog.setText1("Building Scene: " + thisScene.title()) ;
        connect(&img, SIGNAL(percentUpdate(int)), this, SLOT(handleBuildHiresPercentUpdate(int))) ;
        err=DoBuild(thisScene.filename(), &img, i, n, false, false, true, true) ;
        disconnect(&img, SIGNAL(percentUpdate(int)), this, SLOT(handleBuildHiresPercentUpdate(int))) ;
        m_buildHiresPos+=100 ;
    }
    m_prog.hide() ;
    if (err!=PM::Ok) {
        QMessageBox::critical(NULL, "Build Hi-res Error", PM::errString(err)) ;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// doBuild
//
PM::Err MainWindow::DoBuild(QString file, SceneImage *scene, int seq, int of, bool loadpreview, bool buildpreview, bool scaleforpreview, bool buildonly)
{
    PM::Err err = PM::Ok ;
    m_sceneNum = seq ;
    connect(scene, SIGNAL(progressUpdate(QString)), this, SLOT(handleProgressUpdate(QString))) ;
    connect(&m_prog, SIGNAL(abortPressed()), scene, SLOT(handleAbort())) ;
    err = scene->loadImage(file, loadpreview, buildpreview, scaleforpreview, buildonly) ;
    disconnect(&m_prog, SIGNAL(abortPressed()), scene, SLOT(handleAbort())) ;
    disconnect(scene, SIGNAL(progressUpdate(QString)), this, SLOT(handleProgressUpdate(QString))) ;
    return err ;
}

void MainWindow::handleProgressUpdate(QString message)
{
    m_prog.setText2(message) ;
}


//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
//======================================================================================================================


void MainWindow::on_action_Properties_triggered()
{
    TourPropertiesDialog prop ;
    prop.load(&project) ;
    prop.show() ;
    prop.exec() ;
    prop.hide() ;
    prop.save(&project) ;
}


void MainWindow::on_action_About_triggered()
{
    AboutDialog dlg ;
    dlg.setVersion(VERSION, VERSIONDATE) ;
    dlg.exec() ;
    dlg.hide() ;
}

//======================================================================================================================
//
// Scene and Node Menu Buttons
//
void MainWindow::on_action_Add_Scene_triggered()
{
    on_addscene_pushButton_clicked();
}

void MainWindow::on_action_Delete_Scene_triggered()
{
    on_deletescene_pushButton_clicked();
}

void MainWindow::on_action_Add_Node_triggered()
{
    on_nodeAdd_pushButton_clicked();
}

void MainWindow::on_action_Delete_Node_triggered()
{
    on_nodeDelete_pushButton_clicked();
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================


void MainWindow::on_action_Web_Server_triggered()
{
    webserver.show() ;
}

void MainWindow::on_actionE_xit_triggered()
{
    // Save the webserver state
    settings->setValue("webservercommand", webserver.command()) ;
    settings->setValue("webserverfolder", webserver.serverFolder()) ;

    this->close() ;
}

