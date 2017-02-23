/******************************************************
*
*     ©keithhedger Thu 23 Feb 16:14:23 GMT 2017
*     kdhedger68713@gmail.com
*
*     connect.h
* 
******************************************************/

#include <gtk/gtk.h>

#ifndef _CONNECT_
#define _CONNECT_

enum {smbtag=0,ftptag,sshtag,davtag};

enum {smbtagID=3000,ftptagID,sshtagID,davtagID};
enum {usertag=0,passtag,servertag,porttag,sharetag};

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

