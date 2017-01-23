/*
 *
 * ©K. D. Hedger. Thu 17 Nov 13:06:14 GMT 2016 kdhedger68713@gmail.com

 * This file (callbacks.h) is part of KKFileManager.

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


#ifndef _CALLBACKS_
#define _CALLBACKS_

void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct *page);
gboolean buttonDown(GtkWidget *widget,GdkEventButton *event,pageStruct *page);
gboolean buttonDownA(GtkWidget *widget,GdkEventButton *event,pageStruct *page);
void contextMenuActivate(GtkMenuItem *menuitem,contextStruct *ctx);
void selectItem(GtkIconView *icon_view,GtkTreePath *tree_path,pageStruct *page);
void closeTab(GtkButton *button,pageStruct *page);
void openDisk(GtkIconView *icon_view,GtkTreePath *tree_path,gpointer *userdata);
void setPrefs(GtkWidget* widget,gpointer ptr);
void doShutdown(GtkWidget* widget,gpointer data);
gboolean buttonDownDisk(GtkTreeView *widget,GdkEventButton *event,gpointer *userdata);
void runTerminalHere(GtkWidget* widget,gpointer data);
void setAskEntry(GtkWidget* widget,gpointer ptr);
void openBM(GtkIconView *icon_view,GtkTreePath *tree_path,gpointer *userdata);
gboolean buttonDownBM(GtkTreeView *widget,GdkEventButton *event,gpointer *userdata);
gboolean buttonUp(GtkWidget *widget,GdkEventButton *event,pageStruct *page);

void doDragBegin(GtkWidget *widget,GdkDragContext *drag_context,pageStruct *page);
void dragDataGet(GtkWidget *widget,GdkDragContext *context,GtkSelectionData *data,guint info,guint time_,gpointer user_data);
void doDragEnd(GtkWidget *widget,GdkDragContext *drag_context,pageStruct *page);

gboolean dragDrop(GtkWidget *widget,GdkDragContext *context,gint x,gint y,guint time_,gpointer user_data);
void dragDataReceived(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *data,guint info,guint time,gpointer user_data);

void doAbout(GtkWidget *widget,gpointer data);
void externalTool(GtkWidget *widget,gpointer data);

#endif
