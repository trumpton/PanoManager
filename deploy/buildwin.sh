#!/bin/sh

if [ ! -f buildvars.inc ]; then
	echo "Creating a default buildvars.inc.  Please edit / check this file and re-run"
	cp src/buildvars.example.inc buildvars.inc
	exit
fi

pwd

source ./buildvars.inc

PATH=${QTHOME}/bin:${QTINST}/bin:${GCCHOME}/bin:${PATH}

# Clear Old Files

rm -rf repository/*
rm -rf packages/com.trumpton.base/data/*
rm -rf packages/com.trumpton.base.panomanager/data/*
rm -rf packages/com.trumpton.base.vcredist/data/*

# Transfer VC Redist Files

mkdir -p packages/com.trumpton.base.vcredist/data/redist
cp -f src/${VCREDIST} packages/com.trumpton.base.vcredist/data/redist

# Transfer Executable

mkdir -p packages/com.trumpton.base.${REPONAME}/data/bin
cp -f ${SOURCEEXE} packages/com.trumpton.base.${REPONAME}/data/bin

# Fetch Executable support DLLs

(cd packages/com.trumpton.base.${REPONAME}/data/bin; windeployqt.exe ${APPNAME}.exe)

# Generate offline installer

binarycreator.exe --offline-only -p packages -c config/config-win${PROCBIT}-${QTVERSION}.xml repository/${APPNAME}OfflineInstaller.exe

# Generate online installer

binarycreator.exe --online-only -p packages -c config/config-win${PROCBIT}-${QTVERSION}.xml repository/${APPNAME}Installer.exe

# Generate online packages

repogen.exe -p packages repository/${REPONAME}-win${PROCBIT}-${QTVERSION}


