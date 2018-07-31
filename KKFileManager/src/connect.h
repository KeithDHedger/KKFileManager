/*
 *
 * ©K. D. Hedger. Fri 24 Feb 14:14:54 GMT 2017 keithdhedger@gmail.com

 * This file (connect.h) is part of KKFileManager.

 * KKFileManager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKFileManager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKFileManager.  If not, see <http://www.gnu.org/licenses/>.
 */

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

