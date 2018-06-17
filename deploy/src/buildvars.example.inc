
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

PROCBIT=64

#BUILDENV=mingw53_32
#BUILDENV=msvc2015
BUILDENV=msvc2015_64

QTIFWVER=QtIFW-3.0.4

#VCREDIST=vcredist_msvc2015_x86.exe
VCREDIST=vcredist_msvc2015_x64.exe

#
# Paths to EXE File
#

SOURCEEXE=../buildDesktop_Qt_${QTUVERSION}_MSVC2015_64bit2-Release/build/release/${APPNAME}.exe

#
# Path to QT Files
#

QTHOME=/c/Qt/${QTVERSION}/${BUILDENV}
QTINST=/c/Qt/${QTIFWVER}
VCFILE=src/${VCREDIST}
GCCHOME=/c/Qt/Tools/mingw530_32

