
AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

kkfilemanager_SOURCES = $(COMMONSRC)

kkfilemanager_CFLAGS = -O3 -export-dynamic
kkfilemanager_CXXFLAGS = -O3 -export-dynamic

pixfilesdir = $(datadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*.png

scriptfilesdir = $(pkgdatadir)/scripts
scriptfiles_SCRIPTS = ../resources/scripts/*

bin_PROGRAMS = kkfilemanager

man1_MANS = ../resources/man/kkfilemanager.1

docfilesdir = $(pkgdatadir)$(GTK3SUFFIX)/docs
docfiles_DATA = ../resources/docs/*

install-exec-hook:
	mkdir -vp $(DESTDIR)/usr/share/applications $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps ||true
	cp ../resources/pixmaps/KKFileManager.desktop $(DESTDIR)/usr/share/applications/KKFileManager.desktop
	ln -sf $(pkgdatadir)/pixmaps/KKFileManager.png $(DESTDIR)${prefix}/share/icons/hicolor/128x128/apps/KKFileManager.png

uninstall-hook:
	rm -f $(DESTDIR)/usr/share/applications/KKFileManager.desktop
	rm -fr $(pkgdatadir)

