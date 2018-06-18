#!/bin/sh

if [ ! -f buildvars.inc ]; then
	echo "Creating a default buildvars.inc.  Please edit / check this file and re-run"
	cp src/buildvars.example.inc buildvars.inc
	exit
fi

pwd

source ./buildvars.inc

# Clear Old Files

rm -rf repository/*
rm -rf packages64/com.trumpton.base.panomanager64/data/*
rm -rf packages32/com.trumpton.base.panomanager32/data/*




# Transfer 64-bit Executable

mkdir -p packages64/com.trumpton.base.panomanager64/data/bin
cp -f ${SOURCE64EXE} packages64/com.trumpton.base.panomanager64/data/bin

# Fetch 64-bit Executable support DLLs

(cd packages64/com.trumpton.base.panomanager64/data/bin; ${QT64HOME}/bin/windeployqt.exe ${APPNAME}.exe)

# Generate 64-bit offline installer

${QTINST}/bin/binarycreator.exe --offline-only -p packages64 -c config/config-win64-${QTVERSION}.xml repository/${APPNAME}OfflineInstaller64.exe

# Generate online 64-bit installer

${QTINST}/bin/binarycreator.exe --online-only -p packages64 -c config/config-win64-${QTVERSION}.xml repository/${APPNAME}Installer64.exe

# Generate online 64-bit packages

${QTINST}/bin/repogen.exe -p packages64 repository/${REPONAME}-win64-${QTVERSION}





# Transfer 32-bit Executable

mkdir -p packages32/com.trumpton.base.panomanager32/data/bin
cp -f ${SOURCE32EXE} packages32/com.trumpton.base.panomanager32/data/bin

# Fetch 32-bit Executable support DLLs

(cd packages32/com.trumpton.base.panomanager32/data/bin; ${QT32HOME}/bin/windeployqt.exe ${APPNAME}.exe)

# Generate 32-bit offline installer

${QTINST}/bin/binarycreator.exe --offline-only -p packages32 -c config/config-win32-${QTVERSION}.xml repository/${APPNAME}OfflineInstaller32.exe

# Generate online 32-bit installer

${QTINST}/bin/binarycreator.exe --online-only -p packages32 -c config/config-win32-${QTVERSION}.xml repository/${APPNAME}Installer32.exe

# Generate online 32-bit packages

${QTINST}/bin/repogen.exe -p packages32 repository/${REPONAME}-win32-${QTVERSION}

