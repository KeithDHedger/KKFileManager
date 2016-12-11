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

GtkWidget	*tabMenu=NULL;
GtkWidget	*bmContextMenu=NULL;

void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct *page)
{
//printf("folder=%s\n",page->thisFolder);
	switch(event_type)
		{
			case G_FILE_MONITOR_EVENT_UNMOUNTED:
				closeTab(NULL,page);
				updateDiskList();
				break;
			default:
				populatePageStore(page);
		}
}

void contextMenuActivate(GtkMenuItem *menuitem,contextStruct *ctx)
{
	char		buffer[PATH_MAX+20];
	const char	*type="Folder";
	const char	*command="mkdir";
	gchar		*path;
	GtkTreeIter	iter;
	gboolean	isdir;
	int			result=0;
	char		*validFilePath=NULL;
	char		*validDirname=NULL;
	char		*output=NULL;

	switch(ctx->id)
		{
			case CONTEXTNEWFILE:
				type="File";
				command="touch";
			case CONTEXTNEWFOLDER:
				sprintf(buffer,"%s/New %s",ctx->page->thisFolder,type);
				validFilePath=getValidFilepath(buffer);
				fileName=g_path_get_basename(validFilePath);
				doAskForFilename(NULL,NULL);
				if(validName==true)
					{
						validDirname=g_path_get_dirname(validFilePath);
						sprintf(buffer,"%s \"%s/%s\"",command,validDirname,fileName);
						system(buffer);
					}
				free(validFilePath);
				free(validDirname);
				break;
			case CONTEXTBMNEW:
				gtk_list_store_append(bmList,&iter);
				sprintf(buffer,"%s",ctx->page->thisFolder);
				gtk_list_store_set(bmList,&iter,BMPATH,buffer,BMLABEL,basename(buffer),-1);
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
				free(path);
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
				free(path);
				break;
			case CONTEXTEXTRACT:
				gtk_tree_model_get_iter(GTK_TREE_MODEL(ctx->page->listStore),&iter,ctx->treepath);
				gtk_tree_model_get(GTK_TREE_MODEL(ctx->page->listStore),&iter,FILEPATH,&path,ISDIR,&isdir,-1);
				
				sprintf(buffer,"mimetype --output-format=\"%%m\" \"%s\"",path);
				output=oneLiner(buffer);
				buffer[0]=0;
				if((strcmp(output,"application/gzip")==0) || (strcmp(output,"application/x-bzip")==0) || (strcmp(output,"application/x-tar")==0) || (strcmp(output,"application/x-compressed-tar")==0) || (strcmp(output,"application/x-bzip-compressed-tar")==0))
					sprintf(buffer,"(cd \"%s\";tar -xvf \"%s\")",ctx->page->thisFolder,path);
				if(strcmp(output,"application/zip")==0)
					sprintf(buffer,"(cd \"%s\";unzip \"%s\")",ctx->page->thisFolder,path);
				if(strlen(buffer)>0)
					system(buffer);
				free(output);
				free(path);
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
			case CONTEXTBMOPEN:
				gtk_tree_model_get_iter(GTK_TREE_MODEL(bmList),&iter,ctx->treepath);
				gtk_tree_model_get(GTK_TREE_MODEL(bmList),&iter,BMPATH,&path,-1);
				if(path!=NULL)
					{
						pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());
						setCurrentFolderForTab(path,page);
						free(path);
					}
				break;
			case CONTEXTBMNEW:
				printf("CONTEXTBMNEW\n");
				break;
			case CONTEXTBMDELETE:
				printf("CONTEXTBMDELETE\n");
				gtk_tree_model_get_iter(GTK_TREE_MODEL(bmList),&iter,ctx->treepath);
				gtk_list_store_remove(bmList,&iter);
				break;
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

void doDragBegin(GtkWidget *widget,GdkDragContext *drag_context,pageStruct *page)
{
	page->toggleOff=false;
}

gboolean buttonUp(GtkWidget *widget,GdkEventButton *event,pageStruct *page)
{
	GtkTreePath	*treepath;
	GList		*l;

	if(page->stdBehaviour==true)
		return(false);

	l=gtk_icon_view_get_selected_items (page->iconView);

	if((g_list_length(l)>1) && (page->startedDrag)==false)
		{
			if(page->toggleOff==true)
				gtk_icon_view_unselect_all(page->iconView);
		}
	g_list_foreach(l,(GFunc)gtk_tree_path_free,NULL);
	g_list_free(l);
	treepath=gtk_icon_view_get_path_at_pos(page->iconView,event->x,event->y);
	if(treepath!=NULL)
		{
			gtk_icon_view_select_path(page->iconView,treepath);
			gtk_tree_path_free(treepath);
		}
	return(false);
}

gboolean buttonDown(GtkWidget *widget,GdkEventButton *event,pageStruct *page)
{
	GtkWidget	*menuitem;
    GtkTreePath	*treepath;
  
	fromPageID=page->pageID;

	treepath=gtk_icon_view_get_path_at_pos(page->iconView,event->x,event->y);
	if(event->state==0)
		{
			if(treepath==NULL)
				{
					page->toggle=true;
					page->startedDrag=true;
					gtk_icon_view_unselect_all(page->iconView);
					page->toggleOff=false;
				}
			else
				{
					event->state=4;
					page->toggle=false;
					page->startedDrag=false;
					page->toggleOff=true;		
					gtk_icon_view_unselect_path (page->iconView,treepath);
				}
			page->stdBehaviour=false;
		}
	else
		page->stdBehaviour=true;

	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
		{
			gtk_icon_view_unselect_all(page->iconView);
			treepath=gtk_icon_view_get_path_at_pos(page->iconView,event->x,event->y);
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
//extractmenu
					menuitem=newImageMenuItem(CONTEXTEXTRACT,tabMenu);
					contextMenus[CONTEXTEXTRACT]->page=page;
					contextMenus[CONTEXTEXTRACT]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTEXTRACT]);

				}
			else
				{
//new file
					menuitem=newImageMenuItem(CONTEXTNEWFILE,tabMenu);
					contextMenus[CONTEXTNEWFILE]->page=page;
					contextMenus[CONTEXTNEWFILE]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTNEWFILE]);
//new folder
					menuitem=newImageMenuItem(CONTEXTNEWFOLDER,tabMenu);
					contextMenus[CONTEXTNEWFOLDER]->page=page;
					contextMenus[CONTEXTNEWFOLDER]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTNEWFOLDER]);
//new bm
					menuitem=newImageMenuItem(CONTEXTBMNEW,tabMenu);
					contextMenus[CONTEXTBMNEW]->page=page;
					contextMenus[CONTEXTBMNEW]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTBMNEW]);
				}
			gtk_menu_popup(GTK_MENU(tabMenu),NULL,NULL,NULL,NULL,event->button,event->time);
			gtk_widget_show_all((GtkWidget*)tabMenu);
			return(true);
		}
	return(false);
}

gboolean buttonDownBM(GtkTreeView *widget,GdkEventButton *event,gpointer *userdata)
{
	GtkWidget			*menuitem;
    GtkTreePath			*treepath;
	GtkTreeViewColumn	*column;
	GtkTreeSelection	*selection;

	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
		{
			selection=gtk_tree_view_get_selection(widget);
			gtk_tree_view_get_path_at_pos((GtkTreeView*)widget,event->x, event->y,&treepath,&column,NULL,NULL);
			if(G_IS_OBJECT(bmContextMenu))
				{
					g_object_ref_sink(bmContextMenu);
					g_object_unref(bmContextMenu);
				}
			bmContextMenu=gtk_menu_new();
			if(treepath!=NULL)
				{
					gtk_tree_selection_select_path(selection,treepath);
//open
					menuitem=newImageMenuItem(CONTEXTBMOPEN,bmContextMenu);
					contextMenus[CONTEXTBMOPEN]->tree=widget;
					contextMenus[CONTEXTBMOPEN]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextDiskMenuActivate),(void*)contextMenus[CONTEXTBMOPEN]);
//new
					menuitem=newImageMenuItem(CONTEXTBMNEW,bmContextMenu);
					contextMenus[CONTEXTBMNEW]->tree=widget;
					contextMenus[CONTEXTBMNEW]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextDiskMenuActivate),(void*)contextMenus[CONTEXTBMNEW]);
//delete
					menuitem=newImageMenuItem(CONTEXTBMDELETE,bmContextMenu);
					contextMenus[CONTEXTBMDELETE]->tree=widget;
					contextMenus[CONTEXTBMDELETE]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextDiskMenuActivate),(void*)contextMenus[CONTEXTBMDELETE]);

					gtk_menu_popup(GTK_MENU(bmContextMenu),NULL,NULL,NULL,NULL,event->button,event->time);
					gtk_widget_show_all((GtkWidget*)bmContextMenu);
					return(true);
				}
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
//mount
					menuitem=newImageMenuItem(CONTEXTDISKMOUNT,tabMenu);
					contextMenus[CONTEXTDISKMOUNT]->tree=widget;
					contextMenus[CONTEXTDISKMOUNT]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextDiskMenuActivate),(void*)contextMenus[CONTEXTDISKMOUNT]);
//umount
					menuitem=newImageMenuItem(CONTEXTDISKUNMOUNT,tabMenu);
					contextMenus[CONTEXTDISKUNMOUNT]->tree=widget;
					contextMenus[CONTEXTDISKUNMOUNT]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextDiskMenuActivate),(void*)contextMenus[CONTEXTDISKUNMOUNT]);
//eject
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
		}
	else
		{
			asprintf(&command,"mimeopen -L -n \"%s\" &",path);
			system(command);
			free(command);
		}
	free(path);
}

void openBM(GtkIconView *icon_view,GtkTreePath *tree_path,gpointer *userdata)
{
	gchar			*path;
	GtkTreeIter		iter;

	gtk_tree_model_get_iter(GTK_TREE_MODEL(bmList),&iter,tree_path);
	gtk_tree_model_get(GTK_TREE_MODEL(bmList),&iter,BMPATH,&path,-1);
	if(path!=NULL)
		{
			pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());
			setCurrentFolderForTab(path,page);
			free(path);
		}
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

	pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());
	setCurrentFolderForTab(mountpoint,page);

	free(path);
}
#include <libgen.h>

void fileAction(const char *frompath,const char *topath,bool isdir,int action)
{
	char		*command=NULL;
	const char	*recursive;
	char		*newtopath=NULL;
	char		tofilepathbuffer[PATH_MAX];
	char		*frompathfilename=g_path_get_basename(frompath);

	if(isdir==true)
		recursive="-r";
	else
		recursive="";

	sprintf(tofilepathbuffer,"%s/%s",topath,frompathfilename);
	char	*validfilepath=getValidFilepath(tofilepathbuffer);
	char	*tovalidname=g_path_get_basename(validfilepath);
	fileName=tovalidname;

	doAskForFilename(NULL,NULL);
	free(frompathfilename);
	free(validfilepath);

	if(validName==false)
		{
			//free(tovalidname);
			fileName=NULL;
			return;
		}

	sprintf(tofilepathbuffer,"%s/%s",topath,fileName);
	free(tovalidname);
	fileName=NULL;
	if((g_file_test(tofilepathbuffer,G_FILE_TEST_EXISTS)==true) && (validName==true))
		{
			if(yesNo("Really overwrite",tofilepathbuffer)==GTK_RESPONSE_CANCEL)
				return;

		}

	switch(action)
		{
			case GDK_ACTION_COPY:
				asprintf(&command,"cp %s \"%s\" \"%s\"",recursive,frompath,tofilepathbuffer);
				break;
			case GDK_ACTION_LINK:
				asprintf(&command,"ln -sv \"%s\" \"%s\"",frompath,tofilepathbuffer);
				break;
			default:
				if(strcmp(frompath,tofilepathbuffer)!=0)
					asprintf(&command,"mv \"%s\" \"%s\"",frompath,tofilepathbuffer);
				break;
		}
	if(command!=NULL)
		{
			printf("command=%s\n",command);
			system(command);
			free(command);
		}
//	free(uniquetopath);
}

gboolean doDrop(GtkWidget *icon,GdkDragContext *context,int x,int y,unsigned time,pageStruct *page)
{
printf("dand\n");
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

	frompage=getPageStructByIDFromList(fromPageID);
	list=gtk_icon_view_get_selected_items((GtkIconView*)frompage->iconView);

//printf("numitems=%i\n",g_list_length (list));
	treepath=gtk_icon_view_get_path_at_pos(page->iconView,x,y);
	if(treepath!=NULL)
		{
			gtk_tree_model_get_iter(GTK_TREE_MODEL(page->listStore),&iter,treepath);
			gtk_tree_model_get(GTK_TREE_MODEL(page->listStore),&iter,FILEPATH,&topath,ISDIR,&isdir,-1);
			//printf(">>>to path=%s<<<\n",topath);
			retval=true;
		}
	else
		{
			topath=page->thisFolder;
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

			if(terminalCommand!=NULL)
				free(terminalCommand);
			terminalCommand=strdup(gtk_entry_get_text((GtkEntry*)prefsText[TERMINALCOMMANDTXT]));

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
	g_application_quit(mainApp);
}

void runTerminalHere(GtkWidget* widget,gpointer data)
{
	char		*command;
	pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());

	asprintf(&command,"(cd \"%s\";%s\"%s\") &",page->thisFolder,terminalCommand,page->thisFolder);
	system(command);
	free(command);
}

void setAskEntry(GtkWidget* widget,gpointer ptr)
{
	if(fileName!=NULL)
		free(fileName);
	validName=false;

 	if(((long)ptr==-1) && (gtk_entry_get_text_length((GtkEntry*)askentryText[ENTERFILENAMETXT])>0))
 		{
 			fileName=strdup(gtk_entry_get_text((GtkEntry*)askentryText[ENTERFILENAMETXT]));
			validName=true;
		}
	gtk_widget_destroy(askentryWindow);
}
