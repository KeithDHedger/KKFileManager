/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:12:50 GMT 2016 kdhedger68713@gmail.com

 * This file (globals.h) is part of KKFileManager.

 * KKFileManager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKFileManager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKFileManager.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GLOBALS_
#define _GLOBALS_

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <gtk/gtk.h>
#include <magic.h>
#include <map>

#include "config.h"
#include "pages.h"
#include "toolbar.h"
#include "gui.h"
#include "callbacks.h"

#ifdef _USEGTK3_
//menus
//documents
#define GTK_STOCK_NEW "document-new"
#define GTK_STOCK_OPEN "document-open"
#define GTK_STOCK_DELETE "edit-delete"
//odds
#define GTK_STOCK_ADD "list-add"
#define GTK_STOCK_YES "Yes"
#define GTK_STOCK_NO "No"

//icons
#define GTK_STOCK_GO_FORWARD "go-next"
#define GTK_STOCK_GO_BACK "go-previous"
#define GTK_STOCK_GO_UP "go-up"
#define GTK_STOCK_HOME "go-home"
#define GTK_STOCK_DIRECTORY "folder"
#define GTK_STOCK_CLOSE "window-close"
#endif


enum {NEWVBOX=0,NEWHBOX};
enum {TEXT_COLUMN=0,PIXBUF_COLUMN,FILEPATH,ISDIR,NUMCOLS};
enum {DEVPATH=0,DISKNAME,MOUNTPATH,MOUNTED,NUMDISKCOLS};

//main app
extern GtkToolbar		*toolBar;
extern GtkWidget		*mainWindow;
extern GtkWidget		*mainVBox;
extern char				*sinkReturnStr;
extern magic_t			magicInstance;
extern GtkIconTheme		*defaultTheme;
extern GtkIconTheme		*gnomeTheme;
extern GtkNotebook		*mainNotebook;
extern GList			*pageList;
extern GtkAccelGroup	*accgroup;
extern GtkWidget		*mainHPane;
extern GtkWidget		*leftVBox;
extern GtkWidget		*leftVPane;
extern GtkListStore		*diskList;
extern GtkTreeView		*diskView;
extern GtkWidget		*bookmarkList;
extern GtkWidget		*menuBar;

//file menu
extern GtkWidget		*fileMenu;
extern GtkWidget		*menuItemNew;

//tool bar
extern GtkToolItem		*upButton;
extern GtkToolItem		*backButton;
extern GtkToolItem		*forwardButton;
extern GtkToolItem		*homeButton;
extern GtkToolItem		*newButton;
extern GtkToolItem		*locationButton;
extern GtkEntry			*locationTextBox;

//mimes and stuff
extern std::map<unsigned,GdkPixbuf*>	pixBuffCache;
extern GdkPixbuf		*symLink;
extern GdkPixbuf		*brokenLink;
extern GFile			*dirPath;
extern GFileMonitor		*monitorDir;

//tabs
extern unsigned			pageCnt;
extern unsigned			iconSize;
extern unsigned			iconPadding;
extern unsigned			maxTabChars;

//prefs
extern bool				showMenuIcons;
extern char				*toolBarLayout;
extern char				*diskIncludePattern;
extern char				*diskExcludePattern;

//dand
extern unsigned			fromPageID;

//odds
extern int				sinkReturn;

//global functions
char* oneLiner(const char *command);
void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct	*page);
void setCurrentFolderForTab(const char *newfolder,pageStruct *page);
int yesNo(const char *question,char *file);
char *getUniqueFilename(const char *path);

#endif
