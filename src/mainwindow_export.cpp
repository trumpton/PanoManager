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
#include "icons/icons.h"

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
// on_action_Export_Marzipano_triggered
//

void MainWindow::on_action_ExportMarzipano_triggered()
{
    static const char *masks[] = { "f/%y/%x.jpg", "r/%y/%x.jpg", "b/%y/%x.jpg", "l/%y/%x.jpg", "u/%y/%x.jpg", "d/%y/%x.jpg" } ;
    static int exportpreviewsequence[6] = { 2, 5, 0, 3, 1, 4 } ;
    int tileresolution = 256 ;

    QFileInfo exe(QCoreApplication::applicationFilePath()) ;
    QString libFolder = exe.absolutePath() + QString("/../lib/PanoManager/marzipano") ;

    PM::Err err = PM::Ok ;
    QString lastoutputfolder = settings->value("lastoutputfolder", "").toString() ;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Marzipano Output Folder"),
                                                 lastoutputfolder,
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | m_fdOptions);

    if (!checkProject(dir)) {
        return ;
    }

    m_prog.setTitle("Exporing Marzioano") ;
    m_prog.show() ;
    m_prog.setMaximum(200+project.sceneCount()*100);
    m_prog.setValue(0) ;

    lastoutputfolder = dir ;
    settings->setValue("lastoutputfolder", lastoutputfolder) ;

    QJsonObject json ;
    QString marzlist = "" ;

    // Title & Initial scene
    json.insert("name", project.title()) ;
    json.insert("initialscene", project.scene(project.startingSceneId()).titleId());

    // Default Section
    QJsonObject jo_settings ;
    jo_settings.insert("mouseViewMode", "drag") ;
    jo_settings.insert("autorotateEnabled", false) ;
    jo_settings.insert("fullscreenButton", true) ;
    jo_settings.insert("viewControlButtons", true) ;
    json.insert("settings", jo_settings) ;

    QJsonArray ja_scenes ;

    for (int i=0; err==PM::Ok && i<project.sceneCount(); i++) {

        Scene& scene = project.sceneAt(i) ;
        m_prog.setText1( QString("Exporting ") + scene.title()) ;
        int levels, cuberesolution ;
        err = exportFaces(scene, tileresolution, masks, dir + QString("/") + scene.titleId(), &levels, &cuberesolution, 256, exportpreviewsequence) ;

        // List for the html file
        marzlist = marzlist + QString("<a href='#' class='scene' data-id='") +
                scene.titleId() +
                QString("'><li class='text'>") +
                scene.title() +
                QString("</li></a>\n") ;

        QJsonObject jo_scene ;
        jo_scene.insert("id", scene.titleId()) ;
        jo_scene.insert("name", scene.title()) ;
        jo_scene.insert("faceSize", cuberesolution) ;

        QJsonArray ja_levels ;
        for (int l=0; l<levels; l++) {
            QJsonObject jo_level ;
            jo_level.insert("tileSize", tileresolution) ;
            jo_level.insert("size", tileresolution * pow(2,l)) ;
            ja_levels.append(jo_level) ;
            if (l==0) ja_levels.append(jo_level) ; // First level needs repeating for marzipano
        }
        jo_scene.insert("levels", ja_levels) ;

        // initial view when scene opened without link
        QJsonObject jo_initialView ;
        jo_initialView.insert("yaw", 0) ;
        jo_initialView.insert("pitch", 0) ;
        jo_initialView.insert("fov", 1.2) ;
        jo_scene.insert("initialViewParameters", jo_initialView) ;

        QJsonArray ja_links ;
        QJsonArray ja_info ;

        for (int h=0; h<scene.nodeCount(); h++) {

            Node node = scene.nodeAt(h) ;

            QJsonObject jo_node ;
            jo_node.insert("yaw", (node.lon() * 3.141592654*2)/360000) ;
            jo_node.insert("pitch", ((node.lat() * 3.141592654)/180000) * -1) ;
            jo_node.insert("icon", QString("pmicons/") + Icon::uprightIconName(node.type()) + QString(".png")) ;
            jo_node.insert("rotation", (Icon::textureOrientation(node.type())*3.141592654*2)/360) ;
            jo_node.insert("title", node.title()) ;

            if (node.isInfo()) {

                jo_node.insert("text", node.description().replace("\n","br/>")) ;
                ja_info.append(jo_node) ;

            } else if (node.isLink()) {

                Scene dest = project.scene(node.destId()) ;

                jo_node.insert("target", dest.titleId()) ;

                // Initial view when link followed
                QJsonObject jo_destView ;
                jo_destView.insert("yaw", (node.arrivalLon() * 3.141592654*2)/360000) ;
                jo_destView.insert("pitch", (node.arrivalLat() * 3.141592654)/180000) ;
                jo_destView.insert("fov", 1.2) ;
                jo_node.insert("initialViewParameters", jo_destView) ;

                QJsonObject jo_options ;
                jo_options.insert("transitionDuration", 2000) ;
                jo_node.insert("options", jo_options) ;

                ja_links.append(jo_node) ;
            }

        }

        jo_scene.insert("linkHotspots", ja_links) ;
        jo_scene.insert("infoHotspots", ja_info) ;

        ja_scenes.append(jo_scene) ;

    }
    json.insert("scenes", ja_scenes) ;

    if (err==PM::Ok) {
        // Write the configuration file
        m_prog.setText1("Saving Tour Configuration") ;
        QJsonDocument doc ;
        doc.setObject(json);
        QFile configoutput(dir + "/mtour.js") ;
        configoutput.open(QIODevice::WriteOnly | QIODevice::Text) ;
        configoutput.write(QString("var APP_DATA = ").toLatin1()) ;
        configoutput.write(doc.toJson()) ;
        configoutput.close() ;
        m_prog.addValue(100) ;
    }

    if (err==PM::Ok) {
        // Write the Marzipano Supporting Files
        m_prog.setText1("Saving Marzipano Files");
        if (!copyResourceFolder(libFolder, dir, project.overwriteLibrary()))
            err=PM::UnableToTransferResourceFiles ;
    }

    if (err==PM::Ok) {
        // Write the Marzipano Supporting Files
        m_prog.setText1("Saving Custom Icons");
        if (!copyResourceIcons(dir + QString("/pmicons"), 256, true)) {
            err=PM::UnableToTransferResourceFiles ;
        }
    }

    if (err==PM::Ok) {
        // mtour.html is a special case file, where $TITLE$ and $MPSCENESLIST$ are parsed
        QFile htmlin(libFolder + QString("/mtour.html")) ;
        htmlin.open(QIODevice::ReadOnly) ;
        QString htmldata(htmlin.readAll()) ;
        htmlin.close() ;
        QFile htmlout(dir + QString("/mtour.html")) ;
        htmlout.open(QIODevice::WriteOnly) ;
        if (!htmlout.write(htmldata.replace("$MPSCENESLIST$", marzlist).replace("$TITLE$",project.title()).toLatin1())>0) {
            err=PM::UnableToTransferResourceFiles ;
        }
        htmlout.close() ;
    }

    m_prog.hide() ;

    if (err!=PM::Ok) {
        QMessageBox::critical(nullptr, QString("Error Exporting Tour: "), PM::errString(err)) ;
    }

}

//----------------------------------------------------------------------------------------------------------------------
//
// on_action_ExportPanellum_triggered
//

void MainWindow::on_action_ExportPanellum_triggered()
{
    static const char *masks[] = { "f/%y/%x.jpg", "r/%y/%x.jpg", "b/%y/%x.jpg", "l/%y/%x.jpg", "u/%y/%x.jpg", "d/%y/%x.jpg" } ;
    static int exportpreviewsequence[6] = { 2, 5, 0, 3, 1, 4 } ;
    int tileresolution = 256 ;

    QFileInfo exe(QCoreApplication::applicationFilePath()) ;
    QString libFolder = exe.absolutePath() + QString("/../lib/PanoManager/pannellum") ;

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
        err = exportFaces(scene, tileresolution, masks, dir + QString("/") + scene.titleId(), &levels, &cuberesolution, 256, exportpreviewsequence) ;

        QJsonObject jo_scene ;
        jo_scene.insert("northoffset", scene.northOffset()/1000) ;
        jo_scene.insert("title", scene.title()) ;
        jo_scene.insert("preview", "/1/f/0/0.jpg") ;
        jo_scene.insert("type", "multires") ;

        QJsonObject jo_multires ;
        jo_multires.insert("basePath", QString("./") + scene.titleId()) ;
        jo_multires.insert("path", "/%l/%s/%y/%x") ;
        jo_multires.insert("fallbackPath", "/1/%s/%y/%x") ;
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
        QFile configoutput(dir + "/ptour.js") ;
        configoutput.open(QIODevice::WriteOnly | QIODevice::Text) ;
        configoutput.write(QString("var tourdata = ").toLatin1()) ;
        configoutput.write(doc.toJson()) ;
        configoutput.close() ;
        m_prog.addValue(100) ;
    }

    if (err==PM::Ok) {
        // Write the Panellum Supporting Files
        m_prog.setText1("Saving Panellum Files");
        if (!copyResourceFolder(libFolder, dir, project.overwriteLibrary()))
            err=PM::UnableToTransferResourceFiles ;
    }

    if (err==PM::Ok) {
        // ptour.html is a special case file, where $TITLE$ is parsed
        QFile htmlin(libFolder + QString("/ptour.html")) ;
        htmlin.open(QIODevice::ReadOnly) ;
        QString htmldata(htmlin.readAll()) ;
        htmlin.close() ;
        QFile htmlout(dir + QString("/ptour.html")) ;
        htmlout.open(QIODevice::WriteOnly) ;
        if (!(htmlout.write(htmldata.replace("$TITLE$",project.title()).toLatin1())>0)) {
            err=PM::UnableToTransferResourceFiles ;
        }
        htmlout.close() ;
    }


    m_prog.hide() ;

    if (err!=PM::Ok) {
        QMessageBox::critical(nullptr, QString("Error Exporting Tour: "), PM::errString(err)) ;
    }
}


//----------------------------------------------------------------------------------------------------------------------
//
// copyResourceIcons
//

bool MainWindow::copyResourceIcons(QString destfolder, int size, bool ignorerotated)
{
    bool success=true ;
    QDir icondir ;
    icondir.mkpath(destfolder) ;
    for (int i=(int)Icon::WInfo; i<=(int)Icon::BMusic; i++) {
        if (Icon::textureOrientation((Icon::IconType)i)==0 || !ignorerotated) {
            QImage icon(Icon::textureFile((Icon::IconType)i)) ;
            success &= icon.scaled(size, size).
                    save(destfolder +
                         QString("/") +
                         Icon::name((Icon::IconType)i) +
                        QString(".png")) ;
    }
    }
    return success ;
}

//----------------------------------------------------------------------------------------------------------------------
//
// copyResourceFolder
//

bool MainWindow::copyResourceFolder(QString source, QString dest, bool forceOverwrite)
{
    bool success = true ;
    QDir d ;
    d.mkdir(dest) ;

    QFile config(source + "/files.lst") ;
    config.open(QIODevice::ReadOnly) ;
    QString fileData(config.readAll()) ;
    config.close() ;
    QStringList files = fileData.replace("\r","\n").replace("\n\n","\n").split("\n") ;

    if (files.isEmpty()) success = false ;

    foreach (QString file, files) {

        if (!file.isEmpty()) {
            success &= copyFile(source + QString("/") + file, dest + QString("/") + file, forceOverwrite) ;
        }
    }

    return success ;
}

bool MainWindow::copyFile(QString source, QString dest, bool forceOverwrite)
{
    if (!QFile::exists(dest) || forceOverwrite) {
        QFile::remove(dest) ;
        QFileInfo fi(dest) ;
        QString folder = fi.absolutePath() ;
        QDir dir ;
        dir.mkpath(folder) ;
        return QFile::copy(source, dest) ;
    } else {
        return true ;
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

PM::Err MainWindow::exportFaces(Scene scene, int tilesize, const char *masks[], QString folder, int *levels, int *cuberesolution, int previewwidth, int *previewsequence)
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

    if (err==PM::Ok) {
        err=sceneimg.exportVerticalPreview(previewwidth, previewsequence, folder + QString("/preview.jpg")) ;
    }

    disconnect(&m_prog, SIGNAL(abortPressed()), &sceneimg, SLOT(handleAbort())) ;
    disconnect(&sceneimg, SIGNAL(progressUpdate(QString)), this, SLOT(handleProgressUpdate(QString))) ;

    return err ;
}


void MainWindow::exportMarzipanoFiles(QString folder, QString title)
{

}
