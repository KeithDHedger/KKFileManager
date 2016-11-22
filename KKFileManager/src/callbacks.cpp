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

//#include <gtk/gtk.h>
//#include <gdk/gdkkeysyms.h>
//#include <gdk/gdk.h>

#include "globals.h"
//#include "gui.h"

GtkWidget	*tabMenu;

void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct *page)
{
//printf("folder=%s\n",page->thisFolder);
	if((G_FILE_MONITOR_EVENT_CHANGED==event_type) || (G_FILE_MONITOR_EVENT_DELETED==event_type) || (G_FILE_MONITOR_EVENT_CREATED==event_type) || (G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED==event_type) || (G_FILE_MONITOR_EVENT_MOVED==event_type))
		populatePageStore(page);
}

void contextMenuActivate(GtkMenuItem *menuitem,contextStruct *ctx)
{
	char		buffer[PATH_MAX+20];
	const char	*type="Folder";
	const char	*command="mkdir";
	unsigned	cnt=0;
	gchar		*path;
	GtkTreeIter	iter;
	gboolean	isdir;
	int			result=0;

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
				gtk_tree_model_get_iter(GTK_TREE_MODEL(ctx->page->listStore),&iter,ctx->treepath);
				gtk_tree_model_get(GTK_TREE_MODEL(ctx->page->listStore),&iter,FILEPATH,&path,ISDIR,&isdir,-1);

				result=yesNo("Really delete\n",path);
				if(result==GTK_RESPONSE_YES)
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

	fromPageID=page->pageID;
	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
		{
			gtk_icon_view_unselect_all(page->iconView);
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

void selectItem(GtkIconView *icon_view,GtkTreePath *tree_path,pageStruct *page)
{
	printf("clicked\n");
	gchar			*path;
	GtkTreeIter		iter;
	gboolean		isdir;
	char			*command;

	gtk_tree_model_get_iter(GTK_TREE_MODEL(page->listStore),&iter,tree_path);
	gtk_tree_model_get(GTK_TREE_MODEL(page->listStore),&iter,FILEPATH,&path,ISDIR,&isdir,-1);
	//printf("path=%s\n",path);
	//printf("---%i\n",isdir);
	if(isdir==true)
		{
			free(page->thisFolder);
			page->thisFolder=strdup(path);
			gtk_entry_set_text(locationTextBox,page->thisFolder);
			gtk_editable_set_position((GtkEditable*)locationTextBox,-1);
			populatePageStore(page);
			monitorFolderForPage(page);
		}
	else
		{
			asprintf(&command,"mimeopen -L -n \"%s\" &",path);
			system(command);
			free(command);
		}
	free(path);
}

void fileAction(const char *frompath,const char *topath,bool isdir,int action)
{
	char		*command=NULL;
	const char	*recursive;
	char		*uniquetopath;
	char		*newtopath=NULL;

	if(isdir==true)
		recursive="-r";
	else
		recursive="";

	if(g_file_test(topath,G_FILE_TEST_IS_DIR)==true)
		{
			newtopath=g_path_get_basename(frompath);
			asprintf(&command,"%s/%s",topath,newtopath);
			g_free(newtopath);
			uniquetopath=getUniqueFilename(command);
			g_free(command);
		}
	else
		uniquetopath=getUniqueFilename(topath);

	switch(action)
		{
			case GDK_ACTION_COPY:
				asprintf(&command,"cp %s \"%s\" \"%s\"",recursive,frompath,uniquetopath);
				break;
			case GDK_ACTION_LINK:
				asprintf(&command,"ln -sv \"%s\" \"%s\"",frompath,uniquetopath);
				break;
			default:
				asprintf(&command,"mv \"%s\" \"%s\"",frompath,uniquetopath);
				break;
		}
	if(command!=NULL)
		{
			printf("command=%s\n",command);
			system(command);
			free(command);
		}
	free(uniquetopath);
}

#if 0
void updateDuplicates(pageStruct *page)
{
	GList	*list;

	list=pageList;
	while(list!=NULL)
		{
			if(strcmp(page->thisFolder,((pageStruct*)list->data)->thisFolder)==0)
				populatePageStore((pageStruct*)list->data);
			list=list->next;
		}
}

#endif

gboolean doDrop(GtkWidget *icon,GdkDragContext *context,int x,int y,unsigned time,pageStruct *page)
{
//printf("dand\n");
	gchar			*topath;
	gchar			*frompath;
	GtkTreeIter		iter;
	gboolean		isdir;
    GtkTreePath		*treepath;
	GList			*list=NULL;
	bool			retval=false;
	GdkDragAction	action;

//	printf("to pageid=%u\n",page->pageID);
//	printf("from pageid=%u\n",fromPageID);

	action=gdk_drag_context_get_actions(context);
//	printf("action=%u copy=%u move=%u link=%u default=%u\n",action,GDK_ACTION_COPY,GDK_ACTION_MOVE,GDK_ACTION_LINK,GDK_ACTION_DEFAULT);
	pageStruct	*frompage;
//	pageStruct	*topage;

	frompage=getPageStructByIDFromList(fromPageID);
	list=gtk_icon_view_get_selected_items((GtkIconView*)frompage->iconView);

	treepath=gtk_icon_view_get_path_at_pos(page->iconView,x,y);
	if(treepath!=NULL)
		{
			gtk_tree_model_get_iter(GTK_TREE_MODEL(page->listStore),&iter,treepath);
			gtk_tree_model_get(GTK_TREE_MODEL(page->listStore),&iter,FILEPATH,&topath,ISDIR,&isdir,-1);
			//printf(">>>to path=%s<<<\n",topath);
			//topage=page;
			retval=true;
		}
	else
		{
			topath=page->thisFolder;
			//topage=page;
		//printf(">>>to path=%s<<<\n",topath);
		}

	while(list!=NULL)
		{
			gtk_tree_model_get_iter((GtkTreeModel *)frompage->listStore,&iter,(GtkTreePath *)list->data);
			gtk_tree_model_get(GTK_TREE_MODEL(frompage->listStore),&iter,FILEPATH,&frompath,ISDIR,&isdir,-1);
			//printf(">>>from path=%s<<<\n",frompath);
			fileAction(frompath,topath,isdir,action);
			list=list->next;
		}
	g_list_foreach(list,(GFunc)gtk_tree_path_free,NULL);
	g_list_free(list);
    return(retval);
}

