/******************************************************
*
*     ©keithhedger Wed  1 Mar 17:27:33 GMT 2017
*     kdhedger68713@gmail.com
*
*     filenamedialog.h
* 
******************************************************/

#include <gtk/gtk.h>

#ifndef _FILENAMEDIALOG_
#define _FILENAMEDIALOG_

enum {FILENAMETXT=0};

#define _AskFilename_ISDIALOG_
extern char *fileName;
extern bool validName;

extern GtkWidget	*filenamedialogText[];

extern GtkWidget	*filenamedialogWindow;

void doAskForFilename(GtkWidget* widget,gpointer data);

#endif

