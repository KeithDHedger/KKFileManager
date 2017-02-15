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
//#include <magic.h>
#include <map>

#include "config.h"
#include "pages.h"
#include "toolbar.h"
#include "gui.h"
#include "callbacks.h"
#include "prefs.h"
#include "dialogs.h"
#include "history.h"
#include "internet.h"
#include "files.h"
#include "properties.h"
#include "mimetype.h"

#ifdef _USEGTK3_
//menus
//documents
#define GTK_STOCK_NEW "document-new"
#define GTK_STOCK_OPEN "document-open"
#define GTK_STOCK_ABOUT "help-about"
#define GTK_STOCK_DELETE "edit-delete"
#define GTK_STOCK_COPY "edit-copy"
#define GTK_STOCK_PASTE "edit-paste"
//odds
#define GTK_STOCK_ADD "list-add"
#define GTK_STOCK_YES "Yes"
#define GTK_STOCK_NO "No"

//icons
#define GTK_STOCK_GO_FORWARD "go-next"
#define GTK_STOCK_GO_BACK "go-previous"
#define GTK_STOCK_GO_UP "go-up"
#define GTK_STOCK_REFRESH "view-refresh"
#define GTK_STOCK_HOME "go-home"
#define GTK_STOCK_DIRECTORY "folder"
#define GTK_STOCK_CLOSE "window-close"
#define GTK_STOCK_HARDDISK "drive-harddisk"
#define GTK_STOCK_QUIT "application-exit"
#define GTK_STOCK_PREFERENCES "preferences-system"
#define GTK_STOCK_EXECUTE "system-run"
#define GTK_STOCK_CONVERT "stock_convert"
#define GTK_STOCK_PROPERTIES "document-properties"
#define GTK_STOCK_ZOOM_100 "zoom-original"
#define GTK_STOCK_NETWORK "network-workgroup"


#endif

#define ICONSIZE 48

#define	APPEXECNAME "kkfilemanager" _EXECSUFFIX_
#define APPFOLDENAME "." PACKAGE _EXECSUFFIX_
#define GDOCSFOLDER  DATADIR _EXECSUFFIX_ "/docs"
#define ABOUTICON "KKFileManager" _EXECSUFFIX_
#define GTOOLSFOLDER DATADIR _EXECSUFFIX_ "/tools"
#define NETHISTORYFILE "nethistory"
#define MAXNETURLS 8

#define LOADICONCNT 128
#define DRAG_TEXT_URI_LIST 1
#define DROP_TEXT_URI_LIST 1
#define DROP_TEXT_PLAIN    2

#define DIALOGAPPLY -1
#define DIALOGCANCEL -2

struct	args
{
	const char	*name;
	int			type;
	void		*data;
};

struct varStrings
{
	char		*name;
	char		*data;
};

struct toolStruct
{
	char				*menuName;
	char				*filePath;
	char				*command;
	int					flags;
	bool				inTerminal;					
	bool				inPopUp;
	bool				inMenu;
	bool				inBoth;
	bool				global;
};

struct networkDriveStruct
{
	char	* url;
	char	* fstype;
	char	* host;
	char	* port;
	char	* user;
	char	* pass;
	char	* path;
};

enum {NOTANETDRIVE=0,VALIDURL,INVALIDURL};
enum {NEWVBOX=0,NEWHBOX};
enum {TEXT_COLUMN=0,PIXBUF_COLUMN,FILEPATH,ISDIR,NUMCOLS};
enum {DEVPIXBUF=0,DEVPATH,DISKNAME,MOUNTPATH,MOUNTED,NUMDISKCOLS};
enum {BMPIXBUF=0,BMLABEL,BMPATH,NUMBMS};
enum {NOERR=0,NOOPENFILE,NOSAVEFILE};
enum {TYPEINT=1,TYPESTRING,TYPEBOOL,TYPELIST};
enum {HOMEPB=0,DESKTOPPB,COMPUTERPB,BOOKMARKPB,USBDISKPB,HDDRIVEPB,ROMPB,DVDROMPB,CDROMPB,NETWORKDIVE,NUMPBS};

//main app
extern GtkToolbar		*toolBar;
extern GtkWidget		*toolBarBox;
extern GtkWidget		*mainWindow;
extern GtkWidget		*mainVBox;
extern char				*sinkReturnStr;
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
extern GtkListStore		*bmList;
extern GtkTreeView		*bmView;
extern GtkWidget		*menuBar;
extern int				sessionID;
extern GApplication		*mainApp;
extern bool				openDefault;
extern GtkClipboard		*mainClipboard;
extern GValue			boolVal;

//file menu
extern GtkWidget		*fileMenu;
extern GtkWidget		*menuItemNew;
extern GtkWidget		*menuItemTerminal;
extern GtkWidget		*menuItemPrefs;

//tools menu
extern GtkWidget		*toolsMenu;
extern GtkWidget		*toolsSubMenu;
extern GList			*toolsList;
extern args				tool_vars[];
extern int				intermarg;
extern int				flagsarg;
extern int				inpopup;
extern int				inmenu;
extern int				inboth;
extern char				*commandarg;
extern char				*menuname;

//help menu
extern GtkWidget		*helpMenu;
extern GtkWidget		*aboutMenu;

//tool bar
extern GtkToolItem		*upButton;
extern GtkToolItem		*refreshButton;
extern GtkToolItem		*connectButton;
extern GtkToolItem		*backButton;
extern GtkToolItem		*forwardButton;
extern GtkToolItem		*homeButton;
extern GtkToolItem		*newButton;
extern GtkToolItem		*locationButton;
extern GtkEntry			*locationTextBox;

//mimes and stuff
extern std::map<unsigned,GdkPixbuf*> pixBuffCache;
extern GFile			*dirPath;
extern GFileMonitor		*monitorDir;

//tabs
extern unsigned			pageCnt;
extern unsigned			iconSize;
extern unsigned			iconSize3;
extern unsigned			iconPadding;
extern unsigned			maxTabChars;

//menus
//extern GtkWidget		*backButtonMenu;

//prefs
extern bool				showMenuIcons;
extern char				*toolBarLayout;
extern char				*diskIncludePattern;
extern char				*diskExcludePattern;
extern char				*terminalCommand;
extern bool				showHidden;

//save and load var lists
extern char				*windowAllocData;
extern int				windowX;
extern int				windowY;
extern int				windowWidth;
extern int				windowHeight;
extern int				leftPaneWidth;
extern int				leftPaneHeight;
extern args				kkfilemanager_rc[];

//dand
extern unsigned			fromPageID;

//odds
extern int				sinkReturn;
extern char				*netDiskArray[MAXNETURLS];

//tests
extern GdkPixbuf		*testpb;

//pixmaps
extern GdkPixbuf		*genericText;
extern GdkPixbuf		*diskPB;
extern GdkPixbuf		*bookmarkPB;
extern GdkPixbuf		*homePB;
extern GdkPixbuf		*computerPB;
extern GdkPixbuf		*symLink;
extern GdkPixbuf		*brokenLink;
extern GdkPixbuf		*guiPixbufs[NUMPBS];

//global functions
char* oneLiner(const char *command);
void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct	*page);
void setCurrentFolderForTab(const char *newfolder,pageStruct *page,bool updatenav,bool force);
int yesNo(const char *question,char *file);
void information(const char *info,char *file);
int loadVarsFromFile(char *filepath,args *dataptr);
void writeExitData(void);
char *selectionToString(const char *seperator);
unsigned selectionToArray(char ***array,bool touri);
bool checkAccess(const char *path);
int parseNetworkUrl(const char *url,networkDriveStruct* netmount);
void clearNMStruct(networkDriveStruct *netmount);
void printDriveDetails(networkDriveStruct *netmount);

#endif
