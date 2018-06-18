
#
# Application Name
#

APPNAME=PanoManager

#
# Online Repository Name
#

REPONAME=panomanager

#
# Versions
#

QTVERSION=5.10.1
QTUVERSION=5_10_1
QTIFWVER=QtIFW-3.0.4

BUILDENV32=msvc2015
BUILDENV64=msvc2015_64

#
# Paths to EXE File
#

SOURCE64EXE=../buildDesktop_Qt_${QTUVERSION}_MSVC2015_64bit2-Release/build/release/${APPNAME}.exe
SOURCE32EXE=../buildDesktop_Qt_${QTUVERSION}_MSVC2015_32bit-Release/build/release/${APPNAME}.exe


#
# Path to QT Files
#

QT32HOME=/d/Qt/${QTVERSION}/${BUILDENV32}
QT64HOME=/d/Qt/${QTVERSION}/${BUILDENV64}
QTINST=/d/Qt/${QTIFWVER}
VCFILE=src/${VCREDIST}



