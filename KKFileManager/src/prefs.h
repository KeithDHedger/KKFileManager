/******************************************************
*
*     ©keithhedger Thu 24 Nov 20:13:53 GMT 2016
*     kdhedger68713@gmail.com
*
*     prefs.h
* 
******************************************************/

#include <gtk/gtk.h>

#ifndef _PREFS_
#define _PREFS_

enum {TOOLBARLAYOUTTXT=0,INCLUDEDISKLISTTXT,EXCLUDEDISKLISTTXT};

extern GtkWidget	*prefsText[];

extern GtkWidget	*prefsWindow;

void doPrefs(GtkWidget* widget,gpointer data);

#endif

