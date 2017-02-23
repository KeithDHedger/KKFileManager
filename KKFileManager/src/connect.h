/******************************************************
*
*     ©keithhedger Thu 23 Feb 13:54:17 GMT 2017
*     kdhedger68713@gmail.com
*
*     connect.h
* 
******************************************************/

#include <gtk/gtk.h>

#ifndef _CONNECT_
#define _CONNECT_

enum {connectRad5=0,connectRad6,connectRad7,connectRad8};

enum {connectRad5ID=3000,connectRad6ID,connectRad7ID,connectRad8ID};
enum {connectTxt1=0,connectTxt2,connectTxt3,connectTxt4};

#define _Connect_ISDIALOG_
extern char *userName;
extern char *password;
extern char *server;
extern char *shareName;
extern char *mountNetCommand;

extern GtkWidget	*connectRadio[];
extern GtkWidget	*connectText[];

extern GtkWidget	*connectWindow;

void doConnect(GtkWidget* widget,gpointer data);

#endif

