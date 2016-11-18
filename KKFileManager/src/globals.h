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

#include <gtk/gtk.h>
#include <magic.h>
#include <map>

#include "config.h"
#include "toolbar.h"
#include "gui.h"
#include "pages.h"
#include "callbacks.h"

#ifdef _USEGTK3_
//menus
//documents
#define GTK_STOCK_NEW "document-new"
//odds
#define GTK_STOCK_ADD "list-add"

//icons
#define GTK_STOCK_GO_FORWARD "go-next"
#define GTK_STOCK_GO_BACK "go-previous"
#define GTK_STOCK_GO_UP "go-up"
#define GTK_STOCK_HOME "go-home"
#endif


enum {NEWVBOX=0,NEWHBOX};
enum {TEXT_COLUMN,PIXBUF_COLUMN,FILEPATH,ISDIR,NUMCOLS};

//main app
extern GtkToolbar	*toolBar;
extern GtkWidget	*window;
extern GtkWidget	*mainVBox;
extern char			*sinkReturnStr;
extern magic_t		magicInstance;
extern char			*toolBarLayout;
extern GtkIconTheme	*defaultTheme;
extern GtkIconTheme	*gnomeTheme;
extern GtkNotebook	*mainNotebook;
extern GList		*pageList;

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

//global functions
char* oneLiner(const char *command);
void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct	*page);
void setCurrentFolderForTab(const char *newfolder,pageStruct *page);

#endif
