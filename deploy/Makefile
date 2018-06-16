
APPNAME=PanoManager
REPONAME=panomanager

default:
	echo "make linux | win32 | clean"

clean:
	rm -rf repository/*
	rm -rf packages/com.trumpton.base/data/*
	rm -rf packages/com.trumpton.base.panomanager/data/*

linux: envcheck linux-copyfiles linux-installer linux-repo linux-offline-installer

win32: envcheck win32-copyfiles win32-installer win32-repo win32-offline-installer

envcheck:
ifndef QTVERSION
	$(error QTVERSION variable not set, for example QTVERSION=5.9.5)
endif


linux-copyfiles:
	/bin/cp ../build*/*/PanoManager packages/com.trumpton.base.panomanager/data

linux-offline-installer:
	binarycreator --offline-only -p packages -c config/config-linux-$(QTVERSION).xml repository/$(APPNAME)OfflineInstaller.bin

linux-installer:
	binarycreator --online-only -p packages -c config/config-linux-$(QTVERSION).xml repository/$(APPNAME)Installer.bin

linux-repo:
	repogen -p packages repository/$(REPONAME)-linux-$(QTVERSION)




win32-offline-installer:
	binarycreator.exe --offline-only -p packages -c config/config-win32-$(QTVERSION).xml repository/$(APPNAME)OfflineInstaller.exe

win32-installer:
	binarycreator.exe --online-only -p packages -c config/config-win32-$(QTVERSION).xml repository/$(APPNAME)Installer.exe

win32-repo:
	repogen -p packages repository/$(REPONAME)-win32-$(QTVERSION)

win32-copyfiles:


