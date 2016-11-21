
AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

kkfilemanager_SOURCES = $(COMMONSRC)

kkfilemanager_CFLAGS = -O3 -export-dynamic
kkfilemanager_CXXFLAGS = -O3 -export-dynamic

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

scriptfilesdir = $(pkgdatadir)/scripts
scriptfiles_SCRIPTS = ../resources/scripts/*

bin_PROGRAMS = kkfilemanager

man1_MANS = ../resources/man/kkfilemanager.1

install-exec-hook:
	cp ../resources/pixmaps/KKFileManager.desktop /usr/share/applications/KKFileManager.desktop
	echo Icon=${prefix}/share/KKFileManager/pixmaps/KKFileManager.png >> /usr/share/applications/KKFileManager.desktop
	echo Exec=${prefix}/bin/kkfilemanager >> /usr/share/applications/KKFileManager.desktop
#	update-menus

uninstall-hook:
	rm -f /usr/share/applications/KKFileManager.desktop
#	update-menus
	rm -fr $(pkgdatadir)

