
APPNAME=PanoManager
REPONAME=panomanager

QTVERSION=5.10.1
QTUVERSION=5_10_1
INSTVER=3.0.4
MINGWVERSION=53

SOURCEEXE=../build/debug/${APPNAME}.exe

QTHOME=/c/Qt/${QTVERSION}/mingw${MINGWVERSION}_32
QTINST=/c/Qt/QtIFW-${INSTVER}
GCCHOME=/c/Qt/Tools/mingw530_32

PATH=${QTHOME}/bin:${QTINST}/bin:${GCCHOME}/bin:${PATH}

# Clear Old Files

rm -rf repository/*
rm -rf packages/com.trumpton.base/data/*
rm -rf packages/com.trumpton.base.panomanager/data/*

# Transfer Executable

mkdir packages/com.trumpton.base.${REPONAME}/data/bin
cp -f ${SOURCEEXE} packages/com.trumpton.base.${REPONAME}/data/bin

# Fetch Executable support DLLs

(cd packages/com.trumpton.base.${REPONAME}/data/bin; windeployqt ${APPNAME}.exe)

# Generate offline installer

binarycreator.exe --offline-only -p packages -c config/config-win32-${QTVERSION}.xml repository/${APPNAME}OfflineInstaller.exe

# Generate online installer

binarycreator.exe --online-only -p packages -c config/config-win32-${QTVERSION}.xml repository/${APPNAME}Installer.exe

# Generate online packages

repogen.exe -p packages repository/${REPONAME}-win32-${QTVERSION}


