/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:12:14 GMT 2016 keithdhedger@gmail.com

 * This file (toolbar.h) is part of KKFileManager.

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


#ifndef _TOOLBAR_
#define _TOOLBAR_

void setUpToolBar(void);
void goNew(GtkWidget *widget,gpointer data);
void refreshView(GtkWidget *widget,gpointer data);
void goLocation(GtkEntry *entry,GdkEvent *event,gpointer data);

#endif
