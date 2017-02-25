/******************************************************
*
*     ©keithhedger Sat 25 Feb 15:39:21 GMT 2017
*     kdhedger68713@gmail.com
*
*     prefs.h
* 
******************************************************/

#include <gtk/gtk.h>

#ifndef _PREFS_
#define _PREFS_

enum {EXECUTEONCLICK=0};

enum {EXECUTEONCLICKID=3000};

enum {TERMINALCOMMANDTXT=0,TOOLBARLAYOUTTXT,INCLUDEDISKLISTTXT,EXCLUDEDISKLISTTXT};

#define _Prefs_ISDIALOG_

extern GtkWidget	*prefsCheck[];

extern GtkWidget	*prefsText[];

extern GtkWidget	*prefsWindow;

void doPrefs(GtkWidget* widget,gpointer data);

#endif

