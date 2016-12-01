/******************************************************
*
*     ©keithhedger Thu  1 Dec 12:35:42 GMT 2016
*     kdhedger68713@gmail.com
*
*     prefs.h
* 
******************************************************/

#include <gtk/gtk.h>

#ifndef _PREFS_
#define _PREFS_

enum {TERMINALCOMMANDTXT=0,TOOLBARLAYOUTTXT,INCLUDEDISKLISTTXT,EXCLUDEDISKLISTTXT};

extern GtkWidget	*prefsText[];

extern GtkWidget	*prefsWindow;

void doPrefs(GtkWidget* widget,gpointer data);

#endif

