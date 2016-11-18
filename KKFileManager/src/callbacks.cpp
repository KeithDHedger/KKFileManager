/*
 *
 * ©K. D. Hedger. Thu 17 Nov 13:06:08 GMT 2016 kdhedger68713@gmail.com

 * This file (callbacks.cpp) is part of KKFileManager.

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
#include <gdk/gdkkeysyms.h>

#include "gui.h"

void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct *page)
{
	if((G_FILE_MONITOR_EVENT_CHANGED==event_type) || (G_FILE_MONITOR_EVENT_DELETED==event_type) || (G_FILE_MONITOR_EVENT_CREATED==event_type) || (G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED==event_type))
		populatePageStore(page);
}

/*
 if event.type == Gdk.EventType.BUTTON_PRESS:
    path = GtkIconView.get_path_at_pos(event.x, event.y)
    if path != None:
        GtkIconView.select_path(path)
        choice = util.get_selection(GtkIconView,GtkListStore)
        if event.button == 3:
            # popup on right click
        elif event.button == 1:
            self.on_GtkIconView_activated(widget,choice)
*/

GtkWidget			*tabMenu;

void contextMenuActivate(GtkMenuItem *menuitem,pageStruct *page)
{
printf("data=%i\n",(long)page);
}

gboolean buttonDown(GtkWidget *widget,GdkEventButton *event,pageStruct *page)
{
	GtkWidget	*menuitem;
    GtkTreeIter real_it;
    GtkTreePath* treepath;
GtkTreeViewColumn *column;
int cellx;
int celly;
//printf("button=%i\n",event->button.button);
//printf("buttonmask=%i\n",GDK_BUTTON3_MASK);
//printf("state=%i\n",event->button.state & GDK_BUTTON1_MASK);

	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
printf("rite\n");
//if(event->any.type==GDK_BUTTON1_PRESSED)
//	printf("button 1 down\n");


	if(event->type==GDK_BUTTON_PRESS)
		{
			//if(event.button == 3
			printf("button down\n");
		}

	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
		{
		gtk_icon_view_unselect_all (page->iconView);
		//gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(page->listStore),event->x, event->y, &treepath,&column,&cellx,&celly);
		treepath=gtk_icon_view_get_path_at_pos (page->iconView,event->x, event->y);
		if(treepath!=NULL)
		{
		gtk_icon_view_select_path (page->iconView,treepath);

			printf("path=%s\n",gtk_tree_path_to_string (treepath));
			tabMenu=gtk_menu_new();
			menuitem=gtk_menu_item_new_with_label("test menu item 1");
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)1);
//		 g_signal_connect (page->iconView,"item-activated",G_CALLBACK(selectItem),page);	

			menuitem=gtk_menu_item_new_with_label("test menu item 2");
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)2);
			gtk_menu_attach_to_widget(GTK_MENU(tabMenu),widget,NULL);
			gtk_menu_popup(GTK_MENU(tabMenu),NULL,NULL,NULL,NULL,event->button,event->time);
			gtk_widget_show_all((GtkWidget*)tabMenu);
		}
		
		}

return(false);
}




