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

#include "globals.h"

GtkWidget	*tabMenu;

void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct *page)
{
printf("folder=%s\n",page->thisFolder);
	switch(event_type)
		{
			case G_FILE_MONITOR_EVENT_UNMOUNTED:
				closeTab(NULL,page);
				updateDiskList();
				break;
			default:
				populatePageStore(page);
		}
//	if((G_FILE_MONITOR_EVENT_CHANGED==event_type) || (G_FILE_MONITOR_EVENT_DELETED==event_type) || (G_FILE_MONITOR_EVENT_CREATED==event_type) || (G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED==event_type) || (G_FILE_MONITOR_EVENT_MOVED==event_type) || (G_FILE_MONITOR_EVENT_UNMOUNTED==event_type))
//		populatePageStore(page);
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
	char		*validFilePath=NULL;
	char		*validDirname=NULL;

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
					{
						if(isdir==true)
							sprintf(buffer,"rm -r \"%s\"",path);
						else
							sprintf(buffer,"rm \"%s\"",path);
						system(buffer);
					}
				break;
			case CONTEXTDUP:
				gtk_tree_model_get_iter(GTK_TREE_MODEL(ctx->page->listStore),&iter,ctx->treepath);
				gtk_tree_model_get(GTK_TREE_MODEL(ctx->page->listStore),&iter,FILEPATH,&path,ISDIR,&isdir,-1);
				if(fileName!=NULL)
					free(fileName);
				validFilePath=getValidFilepath(path);
				fileName=g_path_get_basename(validFilePath);
				doAskForFilename(NULL,NULL);
				if(validName==true)
					{
						validDirname=g_path_get_dirname(path);
						sprintf(buffer,"cp -r \"%s\" \"%s/%s\"",path,validDirname,fileName);
						system(buffer);
					}
				free(validFilePath);
				free(validDirname);
				break;
			default:
				printf("unknown\n");
		}
}

void contextDiskMenuActivate(GtkMenuItem *menuitem,contextStruct *ctx)
{
	gchar		*path;
	GtkTreeIter	iter;
	char		*command;
	char		*mountpoint;
	
	switch(ctx->id)
		{
			case CONTEXTDISKMOUNT:
				gtk_tree_model_get_iter(GTK_TREE_MODEL(diskList),&iter,ctx->treepath);
				gtk_tree_model_get(GTK_TREE_MODEL(diskList),&iter,DEVPATH,&path,MOUNTPATH,&mountpoint,-1);

				if((mountpoint==NULL) ||(strcmp(mountpoint,"…")==0))
					{
						asprintf(&command,"udevil mount /dev/%s",path);
						sinkReturn=system(command);
						if(sinkReturn!=0)
							{
								free(path);
								free(command);
								updateDiskList();
								free(mountpoint);
								return;
							}
						free(path);
						free(command);
						updateDiskList();
						gtk_tree_model_get_iter(GTK_TREE_MODEL(diskList),&iter,ctx->treepath);
						gtk_tree_model_get(GTK_TREE_MODEL(diskList),&iter,DEVPATH,&path,MOUNTPATH,&mountpoint,-1);
						addNewPage(mountpoint);
						free(mountpoint);
					}
				free(path);
				break;
			case CONTEXTDISKUNMOUNT:
				gtk_tree_model_get_iter(GTK_TREE_MODEL(diskList),&iter,ctx->treepath);
				gtk_tree_model_get(GTK_TREE_MODEL(diskList),&iter,DEVPATH,&path,MOUNTPATH,&mountpoint,-1);

				if((mountpoint!=NULL) && (strcmp(mountpoint,"…")!=0))
					{
						asprintf(&command,"udevil umount \"%s\"",mountpoint);
						system(command);
						free(path);
						free(command);
						updateDiskList();
						free(mountpoint);
					}
				free(path);
				break;
			case CONTEXTDISKEJECT:
				gtk_tree_model_get_iter(GTK_TREE_MODEL(diskList),&iter,ctx->treepath);
				gtk_tree_model_get(GTK_TREE_MODEL(diskList),&iter,DEVPATH,&path,MOUNTPATH,&mountpoint,-1);
				if(path!=NULL)
					{
						asprintf(&command,"udevil umount \"%s\";eject \"%s\"",mountpoint,path);
						system(command);
						free(command);
						updateDiskList();
						free(mountpoint);
					}
				free(path);
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
//open file
					menuitem=newMenuItem(CONTEXTOPEN,tabMenu);
					contextMenus[CONTEXTOPEN]->page=page;
					contextMenus[CONTEXTOPEN]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTOPEN]);
//delete file
					menuitem=newMenuItem(CONTEXTDELETE,tabMenu);
					contextMenus[CONTEXTDELETE]->page=page;
					contextMenus[CONTEXTDELETE]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTDELETE]);
//duplicate file
					menuitem=newImageMenuItem(CONTEXTDUP,tabMenu);
					contextMenus[CONTEXTDUP]->page=page;
					contextMenus[CONTEXTDUP]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTDUP]);

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

gboolean buttonDownDisk(GtkTreeView *widget,GdkEventButton *event,gpointer *userdata)
{
	GtkWidget			*menuitem;
    GtkTreePath			*treepath;
	GtkTreeViewColumn	*column;
	GtkTreeSelection	*selection;

	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
		{
			selection=gtk_tree_view_get_selection(widget);
			gtk_tree_view_get_path_at_pos((GtkTreeView*)widget,event->x, event->y,&treepath,&column,NULL,NULL);

			if(G_IS_OBJECT(tabMenu))
				{
					g_object_ref_sink(tabMenu);
					g_object_unref(tabMenu);
				}
			tabMenu=gtk_menu_new();
			if(treepath!=NULL)
				{
					gtk_tree_selection_select_path(selection,treepath);
					menuitem=newImageMenuItem(CONTEXTDISKMOUNT,tabMenu);
					contextMenus[CONTEXTDISKMOUNT]->tree=widget;
					contextMenus[CONTEXTDISKMOUNT]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextDiskMenuActivate),(void*)contextMenus[CONTEXTDISKMOUNT]);
					menuitem=newImageMenuItem(CONTEXTDISKUNMOUNT,tabMenu);
					contextMenus[CONTEXTDISKUNMOUNT]->tree=widget;
					contextMenus[CONTEXTDISKUNMOUNT]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextDiskMenuActivate),(void*)contextMenus[CONTEXTDISKUNMOUNT]);
					menuitem=newImageMenuItem(CONTEXTDISKEJECT,tabMenu);
					contextMenus[CONTEXTDISKEJECT]->tree=widget;
					contextMenus[CONTEXTDISKEJECT]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextDiskMenuActivate),(void*)contextMenus[CONTEXTDISKEJECT]);

					gtk_menu_popup(GTK_MENU(tabMenu),NULL,NULL,NULL,NULL,event->button,event->time);
					gtk_widget_show_all((GtkWidget*)tabMenu);
					return(true);
				}
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
//	printf("path=%s\n",path);
	//printf("---%i\n",isdir);
	if(isdir==true)
		{
			setCurrentFolderForTab(path,page);
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

void openDisk(GtkIconView *icon_view,GtkTreePath *tree_path,gpointer *userdata)
{
	gchar			*path;
	GtkTreeIter		iter;
	char			*command;
	char			*mountpoint;

	gtk_tree_model_get_iter(GTK_TREE_MODEL(diskList),&iter,tree_path);
	gtk_tree_model_get(GTK_TREE_MODEL(diskList),&iter,DEVPATH,&path,MOUNTPATH,&mountpoint,-1);

printf("path=%s, mountpoint=%s\n",path,mountpoint);

	if((mountpoint==NULL) ||(strcmp(mountpoint,"…")==0))
		{
			asprintf(&command,"udevil mount /dev/%s",path);
			system(command);
			free(path);
			free(command);
			updateDiskList();
			gtk_tree_model_get_iter(GTK_TREE_MODEL(diskList),&iter,tree_path);
			gtk_tree_model_get(GTK_TREE_MODEL(diskList),&iter,DEVPATH,&path,MOUNTPATH,&mountpoint,-1);
		}

	addNewPage(mountpoint);
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

printf("numitems=%i\n",g_list_length (list));
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
			printf(">>>from path=%s<<<\n",frompath);
			fileAction(frompath,topath,isdir,action);
			list=list->next;
		}
	g_list_foreach(list,(GFunc)gtk_tree_path_free,NULL);
	g_list_free(list);
    return(retval);
}

void closeTab(GtkButton *button,pageStruct *page)
{
	if(page!=NULL);
		{
//printf("page=%u folder=%s\n",page->pageID,page->thisFolder);
			gtk_notebook_remove_page(mainNotebook,gtk_notebook_page_num(mainNotebook,page->vBox));
			g_object_unref(page->dirPath);
			g_object_unref(page->monitorDir);
			free(page->thisFolder);
			pageList=g_list_remove(pageList,page);
			free(page);
			if(gtk_notebook_get_n_pages(mainNotebook)==0)
				addNewPage((char*)getenv("HOME"));
		}
}

void setPrefs(GtkWidget* widget,gpointer ptr)
{
//\(sda[123456]$\|sda13\)
//*sd[abc][1-9]*
	if((long)ptr==-1)
		{
			if(toolBarLayout!=NULL)
				free(toolBarLayout);
			toolBarLayout=strdup(gtk_entry_get_text((GtkEntry*)prefsText[TOOLBARLAYOUTTXT]));

			gtk_widget_destroy((GtkWidget*)toolBar);
			toolBar=(GtkToolbar*)gtk_toolbar_new();
			setUpToolBar();
			gtk_box_pack_start(GTK_BOX(toolBarBox),(GtkWidget*)toolBar,true,true,0);
			gtk_widget_show_all(toolBarBox);


			if(diskExcludePattern!=NULL)
				free(diskExcludePattern);
			diskExcludePattern=strdup(gtk_entry_get_text((GtkEntry*)prefsText[EXCLUDEDISKLISTTXT]));
			if(diskIncludePattern!=NULL)
				free(diskIncludePattern);
			diskIncludePattern=strdup(gtk_entry_get_text((GtkEntry*)prefsText[INCLUDEDISKLISTTXT]));
			updateDiskList();
		}
	gtk_widget_destroy(prefsWindow);
}

void doShutdown(GtkWidget* widget,gpointer data)
{
	writeExitData();
	gtk_main_quit();
}
