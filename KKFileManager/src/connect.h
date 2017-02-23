/******************************************************
*
*     ©keithhedger Thu 23 Feb 15:38:19 GMT 2017
*     kdhedger68713@gmail.com
*
*     connect.h
* 
******************************************************/

#include <gtk/gtk.h>

#ifndef _CONNECT_
#define _CONNECT_

enum {connectRad6=0,connectRad7,connectRad8,connectRad9};

enum {connectRad6ID=3000,connectRad7ID,connectRad8ID,connectRad9ID};
enum {connectTxt1=0,connectTxt2,connectTxt3,connectTxt4,connectTxt5};

#define _Connect_ISDIALOG_
extern char *userName;
extern char *password;
extern char *server;
extern char *shareName;
extern char *portName;

extern GtkWidget	*connectRadio[];
extern GtkWidget	*connectText[];

extern GtkWidget	*connectWindow;

void doConnect(GtkWidget* widget,gpointer data);

#endif

