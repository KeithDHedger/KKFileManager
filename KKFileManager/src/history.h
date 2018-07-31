/*
 *
 * ©K. D. Hedger. Fri 23 Dec 11:50:35 GMT 2016 keithdhedger@gmail.com

 * This file (history.h) is part of KKFileManager.

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

#ifndef _HISTORY_
#define _HISTORY_

void doGoBack(GtkWidget *widget,GList *lst);
void doGoForward(GtkWidget *widget,GList *lst);
void navigateHistory(GtkToolButton *toolbutton,gpointer data);
void backMenu(GtkMenuToolButton *toolbutton,gpointer data);
void addToHistory(pageStruct *page,char *folder,bool addtoback);
void clearForward(pageStruct *page);
void clearMenu(GtkMenuShell *menushell,gpointer user_data);
void connectMenu(GtkMenuToolButton *toolbutton,gpointer data);
void loadHistory(unsigned what);
void updateNetHistoryFile(const char *newurl,unsigned what);
gboolean locateBarHistory(GtkWidget *widget,GdkEventButton *event,pageStruct *page);

#endif
