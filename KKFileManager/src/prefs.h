/******************************************************
*
*     ©keithhedger Thu 24 Nov 11:31:23 GMT 2016
*     kdhedger68713@gmail.com
*
*     prefs.h
* 
******************************************************/

#include <gtk/gtk.h>

#ifndef _PREFS_
#define _PREFS_

enum {INCLUDEDISKLISTTXT=0,EXCLUDEDISKLISTTXT};

extern GtkWidget	*prefsText[];

extern GtkWidget	*prefsWindow;

void doPrefs(GtkWidget* widget,gpointer data);

#endif

