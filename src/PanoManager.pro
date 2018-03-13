#-------------------------------------------------
#
# Project created by QtCreator 2017-12-17T10:10:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl

TARGET = PanoManager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        mainwindow_export.cpp \
        project/project.cpp \
        project/scene.cpp \
        project/node.cpp \
        icons/icons.cpp \
        sceneimage/sceneimage.cpp \
        sceneimage/face.cpp \
        sceneimage/maptranslation/maptranslation.cpp \
        dialogs/progress/progressdialog.cpp \
        dialogs/tourproperties/tourpropertiesdialog.cpp \
        dialogs/about/aboutdialog.cpp \
        dialogs/webserver/webserver.cpp \
        widgets/sceneview/sceneviewwidget.cpp \
    sceneimage/maptranslation.cpp

HEADERS += \
        mainwindow.h \
        project/project.h \
        project/scene.h \
        project/node.h \
        icons/icons.h \
        sceneimage/sceneimage.h \
        sceneimage/face.h \
        dialogs/progress/progressdialog.h \
        dialogs/tourproperties/tourpropertiesdialog.h \
        dialogs/about/aboutdialog.h \
        dialogs/webserver/webserver.h \
        widgets/sceneview/sceneviewwidget.h \
    errors/pmerrors.h \
    version.h \
    sceneimage/maptranslation.h

FORMS += \
        mainwindow.ui \
        dialogs/progress/progressdialog.ui \
        dialogs/tourproperties/tourpropertiesdialog.ui \
        dialogs/webserver/webserver.ui \
    dialogs/about/aboutdialog.ui

RESOURCES += \
        widgets/sceneview/shaders.qrc \
        icons/texture.qrc \
        library/pannellum.qrc \
        library/fonts.qrc

DISTFILES +=
