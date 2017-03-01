/******************************************************
*
*     ©keithhedger Tue 28 Feb 17:33:12 GMT 2017
*     kdhedger68713@gmail.com
*
*     prefs.h
* 
******************************************************/

#include <gtk/gtk.h>

#ifndef _PREFS_
#define _PREFS_

enum {EXECUTEONCLICK=0,NONETPICS};

enum {EXECUTEONCLICKID=3000,NONETPICSID};

enum {TERMINALCOMMANDTXT=0,TOOLBARLAYOUTTXT,INCLUDEDISKLISTTXT,EXCLUDEDISKLISTTXT,EXCLUDENETPICSON};

#define _Prefs_ISDIALOG_

extern GtkWidget	*prefsCheck[];

extern GtkWidget	*prefsText[];

extern GtkWidget	*prefsWindow;

void doPrefs(GtkWidget* widget,gpointer data);

#endif

