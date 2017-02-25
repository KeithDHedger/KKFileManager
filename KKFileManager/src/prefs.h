/******************************************************
*
*     ©keithhedger Sat 25 Feb 15:24:13 GMT 2017
*     kdhedger68713@gmail.com
*
*     prefs.h
* 
******************************************************/

#include <gtk/gtk.h>

#ifndef _PREFS_
#define _PREFS_

enum {TERMINALCOMMANDTXT=0,TOOLBARLAYOUTTXT,INCLUDEDISKLISTTXT,EXCLUDEDISKLISTTXT};

#define _Prefs_ISDIALOG_

extern GtkWidget	*prefsText[];

extern GtkWidget	*prefsWindow;

void doPrefs(GtkWidget* widget,gpointer data);

#endif

