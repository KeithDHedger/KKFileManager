/******************************************************
*
*     ©keithhedger Thu  1 Dec 17:21:04 GMT 2016
*     kdhedger68713@gmail.com
*
*     dialogs.h
* 
******************************************************/

#include <gtk/gtk.h>

#ifndef _DIALOGS_
#define _DIALOGS_

enum {ENTERFILENAMETXT=0};

#define _ISDIALOG_
extern char *fileName;
extern bool validName;

extern GtkWidget	*askentryText[];

extern GtkWidget	*askentryWindow;

void doAskForFilename(GtkWidget* widget,gpointer data);

#endif

