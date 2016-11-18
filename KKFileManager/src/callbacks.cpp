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
 
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>

//#include "globals.h"
#include "gui.h"

GtkWidget	*tabMenu;

void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct *page)
{
	if((G_FILE_MONITOR_EVENT_CHANGED==event_type) || (G_FILE_MONITOR_EVENT_DELETED==event_type) || (G_FILE_MONITOR_EVENT_CREATED==event_type) || (G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED==event_type))
		populatePageStore(page);
}

void contextMenuActivate(GtkMenuItem *menuitem,contextStruct *ctx)
{
	char		buffer[PATH_MAX+20];
	const char	*type="Folder";
	const char	*command="mkdir";
	unsigned	cnt=0;

//printf("contextid=%u\n",ctx->id);
	switch(ctx->id)
		{
			case CONTEXTNEWFILE:
				type="File";
				command="touch";
			case CONTEXTNEWFOLDER:
				sprintf(buffer,"%s/New %s",ctx->page->thisFolder,type);
				if(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
					{
						cnt=0;
						while(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
							{
								cnt++;
								sprintf(buffer,"%s/New %s %u",ctx->page->thisFolder,type,cnt);
							}
						sprintf(buffer,"%s \"%s/New %s %u\"",command,ctx->page->thisFolder,type,cnt);
					}
				else
					{
						sprintf(buffer,"%s \"%s/New %s\"",command,ctx->page->thisFolder,type);
					}
				system(buffer);
				break;
			case CONTEXTOPEN:
				selectItem(ctx->page->iconView,ctx->treepath,ctx->page);
				break;
			case CONTEXTDELETE:
				printf("do delete\n");
				break;
			default:
				printf("unknown\n");
		}
}

gboolean buttonDown(GtkWidget *widget,GdkEventButton *event,pageStruct *page)
{
	GtkWidget	*menuitem;
    GtkTreePath	*treepath;

	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
		{
			gtk_icon_view_unselect_all (page->iconView);
			treepath=gtk_icon_view_get_path_at_pos (page->iconView,event->x, event->y);
			if(G_IS_OBJECT(tabMenu))
				{
					g_object_ref_sink(tabMenu);
					g_object_unref(tabMenu);
				}
			tabMenu=gtk_menu_new();
			if(treepath!=NULL)
				{
					gtk_icon_view_select_path(page->iconView,treepath);
					menuitem=newMenuItem(CONTEXTOPEN,tabMenu);
					contextMenus[CONTEXTOPEN]->page=page;
					contextMenus[CONTEXTOPEN]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTOPEN]);
					menuitem=newMenuItem(CONTEXTDELETE,tabMenu);
					contextMenus[CONTEXTDELETE]->page=page;
					contextMenus[CONTEXTDELETE]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTDELETE]);
				}
			else
				{
					menuitem=newImageMenuItem(CONTEXTNEWFILE,tabMenu);
					contextMenus[CONTEXTNEWFILE]->page=page;
					contextMenus[CONTEXTNEWFILE]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTNEWFILE]);
					menuitem=newImageMenuItem(CONTEXTNEWFOLDER,tabMenu);
					contextMenus[CONTEXTNEWFOLDER]->page=page;
					contextMenus[CONTEXTNEWFOLDER]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTNEWFOLDER]);
				}
			gtk_menu_popup(GTK_MENU(tabMenu),NULL,NULL,NULL,NULL,event->button,event->time);
			gtk_widget_show_all((GtkWidget*)tabMenu);
			return(true);
		}

	return(false);
}



