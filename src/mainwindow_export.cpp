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
// PanoManager Main Window Export Functions
//

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QtCore/qmath.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

#include "sceneimage/sceneimage.h"
#include "icons/icons.h"
#include "dialogs/progress/progressdialog.h"
#include "errors/pmerrors.h"


//======================================================================================================================
//
// Menu: Export
//
// on_action_ExportMarzipano_triggered    - Export to Marzipano
// on_action_ExportPanellum_triggered     - Export to Panellum
// exportfaces                            - Create the face fragments
//

//----------------------------------------------------------------------------------------------------------------------
//
// on_action_ExportMarzipano_triggered
//

void MainWindow::on_action_ExportMarzipano_triggered()
{
    // Not currently supported
}


//----------------------------------------------------------------------------------------------------------------------
//
// on_action_ExportPanellum_triggered
//

void MainWindow::on_action_ExportPanellum_triggered()
{
    static const char *masks[] = { "f%y_%x.jpg", "r%y_%x.jpg", "b%y_%x.jpg", "l%y_%x.jpg", "u%y_%x.jpg", "d%y_%x.jpg" } ;
    int tileresolution = 256 ;

    PM::Err err = PM::Ok ;
    QString lastoutputfolder = settings->value("lastoutputfolder", "").toString() ;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Panellum Output Folder"),
                                                 lastoutputfolder,
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | m_fdOptions);

    if (!checkProject(dir)) {
        return ;
    }

    m_prog.setTitle("Exporing Panellum") ;
    m_prog.show() ;
    m_prog.setMaximum(200+project.sceneCount()*100);
    m_prog.setValue(0) ;

    lastoutputfolder = dir ;
    settings->setValue("lastoutputfolder", lastoutputfolder) ;
    dir = dir + "/" ;

    QJsonObject json ;

    // Default Section

    QJsonObject jo_default ;
    if (!project.title().isEmpty()) jo_default.insert("title", project.author()) ;
    if (!project.author().isEmpty()) jo_default.insert("author", project.author()) ;
    if (!project.startingSceneId().isEmpty()) {
        jo_default.insert("firstScene", project.scene(project.startingSceneId()).titleId()) ;
        jo_default.insert("pitch", project.startingSceneLat()/1000.0) ;
        jo_default.insert("yaw", project.startingSceneLon()/1000.0) ;
    }
    if (project.sceneFade()>=0) jo_default.insert("sceneFadeDuration", project.sceneFade()) ;
    jo_default.insert("compass", project.compass()) ;
    jo_default.insert("autoLoad", project.autoLoad()) ;
    if (project.autoRotate()>=0) jo_default.insert("autoRotateInactivityDelay", project.autoRotate()) ;
    jo_default.insert("hotSpotDebug", project.debug()) ;
    json.insert("default", jo_default) ;

    // Scenes Section

    QJsonObject jo_scenes ;

    for (int i=0; err==PM::Ok && i<project.sceneCount(); i++) {

        m_prog.setText1( QString("Exporting ") + project.sceneAt(i).title()) ;
        m_prog.setDelta(0) ;

        Scene& scene = project.sceneAt(i) ;
        int levels, cuberesolution ;
        err = exportFaces(scene, tileresolution, masks, dir + scene.titleId(), &levels, &cuberesolution) ;

        QJsonObject jo_scene ;
        jo_scene.insert("northoffset", scene.northOffset()/1000) ;
        jo_scene.insert("title", scene.title()) ;
        jo_scene.insert("preview", "/1/f0_0.jpg") ;
        jo_scene.insert("type", "multires") ;

        QJsonObject jo_multires ;
        jo_multires.insert("basePath", QString("./") + scene.titleId()) ;
        jo_multires.insert("path", "/%l/%s%y_%x") ;
        jo_multires.insert("fallbackPath", "/1/%s0_0") ;
        jo_multires.insert("tileResolution", tileresolution) ;
        jo_multires.insert("maxLevel", levels) ;
        jo_multires.insert("extension", "jpg") ;
        jo_multires.insert("cubeResolution", cuberesolution) ;
        jo_scene.insert("multiRes", jo_multires) ;

        // Hotspots within a scene

        QJsonArray ja_hotspots ;
        for (int j=0; j<scene.nodeCount(); j++) {
            Node& node = scene.nodeAt(j) ;
            QJsonObject jo_hotspot ;
            jo_hotspot.insert("pitch", node.lat()/1000.0) ;
            jo_hotspot.insert("yaw", node.lon()/1000.0) ;
            if (node.isLink()) {
                jo_hotspot.insert("type", "scene") ;
                jo_hotspot.insert("text", node.title()) ;
                jo_hotspot.insert("sceneId", project.scene(node.destId()).titleId()) ;
                jo_hotspot.insert("targetPitch", node.arrivalLat()/1000.0) ;
                jo_hotspot.insert("targetYaw", node.arrivalLon()/1000.0) ;
            } else if (node.isInfo() || node.isMedia() || node.isMusic()){
                jo_hotspot.insert("type", "info") ;
                QString title = node.title() ;
                if (!node.description().isEmpty()) title = title + QString(" - ") + node.description() ;
                jo_hotspot.insert("text", node.title() + QString(" ") + node.description()) ;
                if (!node.url().isEmpty()) { jo_hotspot.insert("URL", node.url()) ; }
            }

            ja_hotspots.append(jo_hotspot) ;
        }
        jo_scene.insert("hotSpots", ja_hotspots) ;
        jo_scenes.insert(scene.titleId(), jo_scene) ;

        m_prog.addValue(100) ;

    }

    json.insert("scenes", jo_scenes) ;


    if (err==PM::Ok) {
        // Write the configuration file
        m_prog.setText1("Saving Tour Configuration") ;
        QJsonDocument doc ;
        doc.setObject(json);
        QFile configoutput(dir + "tour.js") ;
        configoutput.open(QIODevice::WriteOnly | QIODevice::Text) ;
        configoutput.write(QString("var tourdata = ").toLatin1()) ;
        configoutput.write(doc.toJson()) ;
        configoutput.close() ;
        m_prog.addValue(100) ;
    }

    if (err==PM::Ok) {
        // Write the Panellum Supporting Files
        m_prog.setText1("Saving Panellum Files");
        exportPanellumFiles(dir, project.title());
        m_prog.addValue(100) ;
    }

    m_prog.hide() ;

    if (err!=PM::Ok) {
        QMessageBox::critical(nullptr, QString("Error Exporting Tour: "), PM::errString(err)) ;
    }
}


//----------------------------------------------------------------------------------------------------------------------
//
// exportPanellumFiles - Produce the supporting html and script files
//
void MainWindow::exportPanellumFiles(QString folder, QString title)
{
    QFile htmlin(":/pannellum/tour.html") ;
    htmlin.open(QIODevice::ReadOnly) ;
    QString htmldata(htmlin.readAll()) ;
    htmlin.close() ;

    QFile htmlout(folder + QString("tour.html")) ;
    htmlout.open(QIODevice::WriteOnly) ;
    htmlout.write(htmldata.replace("$TITLE$",title).toLatin1()) ;
    htmlout.close() ;

    copyResourceFolder(":/PannellumFles", folder + QString("/pannellum"), project.overwriteLibrary()) ;

}

void MainWindow::copyResourceFolder(QString foldersrc, QString dest, bool forceOverwrite)
{
    QDir d ;
    d.mkdir(dest) ;
    copyFile(":/pannellum/pannellum.js", dest + QString("/pannellum.js"), forceOverwrite) ;
    copyFile(":/pannellum/pannellum.css", dest + QString("/pannellum.css"), forceOverwrite) ;
    copyFile(":/pannellum/changelog.md", dest + QString("/changelog.md"), forceOverwrite) ;
    copyFile(":/pannellum/COPYING", dest + QString("/COPYING"), forceOverwrite) ;
    copyFile(":/pannellum/pannellum.htm", dest + QString("/pannellum.htm"), forceOverwrite) ;
    copyFile(":/pannellum/readme.md", dest + QString("/readme.md"), forceOverwrite) ;
    copyFile(":/pannellum/VERSION", dest + QString("/VERSION"), forceOverwrite) ;
}

void MainWindow::copyFile(QString source, QString dest, bool forceOverwrite)
{
    if (!QFile::exists(dest) || forceOverwrite) {
        QFile::remove(dest) ;
        QFile::copy(source, dest) ;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
// checkProject - Check scene names are unique and all required fields are filled in
//

bool MainWindow::checkProject(QString dir)
{
    if (dir.isEmpty()) {
        // Error, output folder not specified
        QMessageBox::critical(this, "Error", "Output Folder Not Defined.  Please configure in Tour/Properties") ;
        return false ;
    }

    for (int i=0; i<project.sceneCount(); i++) {
        QString name = project.sceneAt(i).titleId() ;
        int matches=0 ;
        for (int j=0; j<project.sceneCount(); j++) {
            if (project.sceneAt(j).titleId().compare(name)==0) {
                matches++ ;
            }
        }
        if (matches!=1) {
            QMessageBox::critical(this, "Error", QString("The follwing scene name exists multiple times: ") + project.sceneAt(i).title()) ;
            return false ;
        }
    }

    if (project.title().isEmpty()) {
        QMessageBox::critical(this, "Error", "Tour Title Not Defined.  Please configure in Tour/Properties") ;
        return false ;
    }

    if (!project.scene(project.startingSceneId()).isValid()) {
        QMessageBox::critical(this, "Error", "Invalid Starting Scene.  Please configure in Tour/Properties") ;
        return false ;
    }

    return true ;
}


//----------------------------------------------------------------------------------------------------------------------
//
// exportFaces - Perform the export
//

PM::Err MainWindow::exportFaces(Scene scene, int tilesize, const char *masks[], QString folder, int *levels, int *cuberesolution)
{
    SceneImage sceneimg ;
    if (!levels || !cuberesolution) return PM::InvalidPointer ;

    m_prog.setDelta(0) ;

    connect(&sceneimg, SIGNAL(progressUpdate(QString)), this, SLOT(handleProgressUpdate(QString))) ;
    connect(&m_prog, SIGNAL(abortPressed()), &sceneimg, SLOT(handleAbort())) ;

    // Load the high resolution version of the image
    PM::Err err = sceneimg.loadImage(scene.filename(), false, false, false, false) ;

    if (err==PM::Ok) {

        m_prog.setDelta(50) ;

        int width = sceneimg.getFace(0).width() ;

        int res=0 ;
        if (tilesize>width) tilesize=width ;
        while ( err==PM::Ok && qPow(2,res)*tilesize <= width) {
            int imagesize = qPow(2,res)*tilesize ;
            for (int f=0; err==PM::Ok && f<6; f++) {
                QString filename = folder + QString("/") + QString::number(res+1) ;
                QString mask = masks[f];
                if (imagesize>width) imagesize=width ;
                err = sceneimg.getFace(f).exportTiles(imagesize, tilesize, filename, mask) ;
                m_prog.setDelta((f*100)/6) ;
            }
            res++ ;
        }

        *cuberesolution = width ;
        *levels = res ;
        m_prog.setDelta(100) ;

    }

    disconnect(&m_prog, SIGNAL(abortPressed()), &sceneimg, SLOT(handleAbort())) ;
    disconnect(&sceneimg, SIGNAL(progressUpdate(QString)), this, SLOT(handleProgressUpdate(QString))) ;

    return err ;
}


void MainWindow::exportMarzipanoFiles(QString folder, QString title)
{

}
