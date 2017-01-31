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
#include <libgen.h>
#include <pwd.h>
#include <grp.h>

#include "globals.h"

GtkWidget	*tabMenu=NULL;
GtkWidget	*bmContextMenu=NULL;
char		*dropPath=NULL;


void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct *page)
{
//printf("folder=%s\n",page->thisFolder);
	switch(event_type)
		{
			case G_FILE_MONITOR_EVENT_UNMOUNTED:
				closeTab(NULL,page);
				updateDiskList();
				break;
			case  G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
				break;
			default:
				populatePageStore(page);
		}
}

void contextMenuActivate(GtkMenuItem *menuitem,contextStruct *ctx)
{
	char			buffer[PATH_MAX+20];
	const char		*type="Folder";
	const char		*command="mkdir";
	gchar			*path;
	GtkTreeIter		iter;
	gboolean		isdir;
	int				result=0;
	char			*output=NULL;
	int				cnt;
	char			*stringlist;
	filePathStruct	fps={NULL,NULL,NULL,NULL,NULL,NULL,false,false,false,false};
	GList			*iconlist;
	char			**selectionarray=NULL;

	switch(ctx->id)
		{
			case CONTEXTNEWFILE:
				type="File";
				command="touch";
			case CONTEXTNEWFOLDER:
				sprintf(buffer,"%s/New %s",ctx->page->thisFolder,type);
				setFilePathStruct(&fps,"fD",buffer,ctx->page->thisFolder);
				fps.askFileName=true;
				getValidToPathFromFilepath(&fps);
				if(fps.modified==true)
					{
						sprintf(buffer,"%s \"%s\"",command,fps.toFilePath);
						system(buffer);
					}
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
				iconlist=gtk_icon_view_get_selected_items(ctx->page->iconView);
				if(iconlist!=NULL)
					{
						while(iconlist!=NULL)
							{
								gtk_tree_model_get_iter(GTK_TREE_MODEL(ctx->page->listStore),&iter,(GtkTreePath*)iconlist->data);
								gtk_tree_model_get(GTK_TREE_MODEL(ctx->page->listStore),&iter,FILEPATH,&path,-1);
								if(path!=NULL)
									{
										setFilePathStruct(&fps,"fDp",path,ctx->page->thisFolder);
										fps.askFileName=true;
										getValidToPathFromFilepath(&fps);
										if(fps.modified==true)
											doFileAction(&fps,GDK_ACTION_COPY);
										free(path);
									}
								iconlist=iconlist->next;
							}
					}
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
//copy
			case CONTEXTCOPY:
				stringlist=selectionToString("\n");
				if(stringlist!=NULL)
					{
						gtk_clipboard_set_text(mainClipboard,stringlist,-1);
						g_free(stringlist);
					}
				break;

//props
			case CONTEXTPROPS:
				selectionarray=selectionToArray(false);
				if(selectionarray!=NULL)
					{
						unsigned		cnt=0;
						struct stat		st;
						int				statret;
						struct passwd	*pws;
						struct group	*grp;
						struct tm		*date;
						while(selectionarray[cnt]!=NULL)
							{
								filePath=selectionarray[cnt];
								statret=stat(filePath,&st);
									if(statret>=0)
										{
											sprintf(buffer,"%i",(int)st.st_size);
											fileSize=strdup(buffer);
											pws=getpwuid(st.st_uid);
											ownerName=pws->pw_name;
											grp=getgrgid(st.st_gid);
											groupName=grp->gr_name;
											date=localtime(&st.st_mtime);
											strftime(buffer,sizeof(buffer),"%a %d-%m-%Y %H:%M:%S",date);
											fileModified=strdup(buffer);
											date=localtime(&st.st_atime);
											strftime(buffer,sizeof(buffer),"%a %d-%m-%Y %H:%M:%S",date);
											fileAccessed=strdup(buffer);
											oReadBit=st.st_mode & S_IRUSR;
											oWriteBit=st.st_mode & S_IWUSR;
											oExecuteBit=st.st_mode & S_IXUSR;
											gReadBit=st.st_mode & S_IRGRP;
											gWriteBit=st.st_mode & S_IWGRP;
											gExecuteBit=st.st_mode & S_IXGRP;
											rReadBit=st.st_mode & S_IROTH;
											rWriteBit=st.st_mode & S_IWOTH;
											rExecuteBit=st.st_mode & S_IXOTH;
											setUIDBit=st.st_mode & S_ISUID;
											setGIDBit=st.st_mode & S_ISGID;
											stickyBit=st.st_mode & S_ISVTX;
											doFileProps(NULL,NULL);
											free(fileSize);
											free(fileModified);
											free(fileAccessed);
										}
								cnt++;
							}
						g_strfreev(selectionarray);
					}
				break;
//paste
			case CONTEXTPASTE:
				path=gtk_clipboard_wait_for_text(mainClipboard);
				if(path!=NULL)
					{
						gchar **array=g_strsplit(path,"\n",-1);
						cnt=0;
						while((array[cnt]!=NULL) && (strlen(array[cnt])>0))
							{
								setFilePathStruct(&fps,"fDp",array[cnt],ctx->page->thisFolder);
								fps.askFileName=true;
								getValidToPathFromFilepath(&fps);
								if(fps.modified==true)
									doFileAction(&fps,GDK_ACTION_COPY);
								cnt++;
							}
						g_strfreev(array);
					}
				free(path);
				break;
			case CONTEXTSHOW:
			case CONTEXTHIDE:
				showHidden=!showHidden;
				refreshView(NULL,NULL);
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
						setCurrentFolderForTab(path,page,true,false);
						clearForward(page);
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

char		**ar;

void doDragBegin(GtkWidget *widget,GdkDragContext *drag_context,pageStruct *page)
{
	ar=selectionToArray(true);
	page->toggleOff=false;
}

void dragDataGet(GtkWidget *widget,GdkDragContext *context,GtkSelectionData *data,guint info,guint time_,gpointer user_data)
{
	if(ar!=NULL)
		{
			if(info==DRAG_TEXT_URI_LIST)
				{
					gtk_selection_data_set_uris(data,ar);
				}
		}
}

void doDragEnd(GtkWidget *widget,GdkDragContext *context,pageStruct *page)
{
	unsigned	cnt=0;

	page->toggleOff=true;
	page->startedDrag=false;
	page->stdBehaviour=false;

#ifndef _USEGTK3_
	gtk_widget_destroy((GtkWidget*)page->iconView);
	newIconView(page);
	gtk_container_add((GtkContainer*)page->scrollBox,(GtkWidget*)page->iconView);
	setCurrentFolderForTab(page->thisFolder,page,true,true);
#endif
	if(ar!=NULL)
		while(ar[cnt]!=NULL)
			free(ar[cnt++]);			
}

gboolean dragDrop(GtkWidget *widget,GdkDragContext *context,gint x,gint y,guint time,gpointer user_data)
{
	GtkTargetList	*targetlist;
	GdkAtom			target;
    GtkTreePath		*treepath;
	GtkTreeIter		iter;
	pageStruct		*page=NULL;

	g_print("<<< DRAG DROP >>>\n");


	if(dropPath!=NULL)
		free(dropPath);
	dropPath=NULL;
	page=getPageFromCurrentTab();
	if(page!=NULL)
		{
			treepath=gtk_icon_view_get_path_at_pos(page->iconView,x,y);
			if(treepath!=NULL)
				{
					gtk_tree_model_get_iter(GTK_TREE_MODEL(page->listStore),&iter,treepath);
					gtk_tree_model_get(GTK_TREE_MODEL(page->listStore),&iter,FILEPATH,&dropPath,-1);
				}
		}
	
	targetlist=gtk_drag_dest_get_target_list(widget);
	target=gtk_drag_dest_find_target(widget,context,targetlist);
	if (target==GDK_NONE)
		{
			gtk_drag_finish(context, FALSE, FALSE, time);
			return(false);
		}

	gtk_drag_get_data(widget,context,target,time);

	return(true);
}

void dragDataReceived(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *data,guint info,guint time,gpointer user_data)
{
	gboolean		result=false;
	gchar			**uris;
	unsigned		cnt;
	char			*filepath=NULL;
	pageStruct		*page;
	filePathStruct	fps={NULL,NULL,NULL,NULL,NULL,NULL,false,false,false,false};

	char			*usethisdroppath=NULL;

	page=(pageStruct*)user_data;
	if(page==NULL)
		{
		 	gtk_drag_finish(context,false,false,time);
		 	return;
		}

	g_print("<<< DRAG DATA RECEIVED >>>\n");

	if(data->length==0 || data->format!=8)
		{
		 	gtk_drag_finish(context,false,false,time);
		 	return;
		 }

	if (info==DRAG_TEXT_URI_LIST)
		{
			uris=gtk_selection_data_get_uris(data);
			if(uris==NULL)
				{
					gtk_drag_finish(context,false,false,time);
					return;
				}

			cnt=0;
			if(dropPath!=NULL)
				usethisdroppath=dropPath;
			else
				usethisdroppath=page->thisFolder;

			while(uris[cnt]!=NULL)
				{
					filepath=g_filename_from_uri(uris[cnt],NULL,NULL);
					result=true;

					//setFilePathStruct(&fps,"fDp",filepath,page->thisFolder);
					setFilePathStruct(&fps,"fDp",filepath,usethisdroppath);
					fps.askFileName=true;
					getValidToPathFromFilepath(&fps);
					if(fps.modified==true)
						doFileAction(&fps,gdk_drag_context_get_suggested_action(context));
					free(filepath);
					cnt++;
				}
			g_strfreev(uris);
		}
	else
		{
			g_print("error\n");
		}

	gtk_drag_finish(context,result,false,time);
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
	GtkWidget	*submenu;
    GtkTreePath	*treepath=NULL;
	GList		*ptr;
	bool		pathfound=false;
	GList		*si=gtk_icon_view_get_selected_items(page->iconView);

	fromPageID=page->pageID;
	treepath=gtk_icon_view_get_path_at_pos(page->iconView,event->x,event->y);

	if(event->state==0)
		{
			page->stdBehaviour=false;
			if(treepath==NULL)
				{
					page->startedDrag=true;
					gtk_icon_view_unselect_all(page->iconView);
					page->toggleOff=false;
				}
			else
				{
					event->state=4;
					page->startedDrag=false;
					page->toggleOff=true;		
					gtk_icon_view_unselect_path(page->iconView,treepath);
				}
		}
	else
		page->stdBehaviour=true;

	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
		{
			if(G_IS_OBJECT(tabMenu))
				{
					g_object_ref_sink(tabMenu);
					g_object_unref(tabMenu);
				}
			tabMenu=gtk_menu_new();
			if(treepath!=NULL)
				{
					if(si!=NULL)
						{
							while(si!=NULL)
								{
									if(si->data!=NULL && treepath!=NULL)
										if(gtk_tree_path_compare((const GtkTreePath*)si->data,treepath)==0)
											pathfound=true;
										si=si->next;
								}
							if(pathfound==false)
								gtk_icon_view_unselect_all(page->iconView);
						}
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
//copy
					menuitem=newImageMenuItem(CONTEXTCOPY,tabMenu);
					contextMenus[CONTEXTCOPY]->page=page;
					contextMenus[CONTEXTCOPY]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTCOPY]);

//props
					menuitem=newImageMenuItem(CONTEXTPROPS,tabMenu);
					contextMenus[CONTEXTPROPS]->page=page;
					contextMenus[CONTEXTPROPS]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTPROPS]);

//show tools					
					menuData[MAINTOOLSBLANKTOOL].cb=NULL;
					menuData[MAINTOOLSBLANKTOOL].stockID=GTK_STOCK_EXECUTE;
					menuData[MAINTOOLSBLANKTOOL].menuLabel="Tools";
					menuitem=newImageMenuItem(MAINTOOLSBLANKTOOL,tabMenu);
					submenu=gtk_menu_new();
					gtk_menu_item_set_submenu((GtkMenuItem*)menuitem,submenu);

					ptr=toolsList;
					while(ptr!=NULL)
						{
							if((((toolStruct*)ptr->data)->inPopUp==true) || (((toolStruct*)ptr->data)->inBoth==true))
								{
									menuData[MAINTOOLSBLANKTOOL].cb=(void*)externalTool;
									menuData[MAINTOOLSBLANKTOOL].stockID=NULL;
									menuData[MAINTOOLSBLANKTOOL].menuLabel=((toolStruct*)ptr->data)->menuName;
									menuData[MAINTOOLSBLANKTOOL].userData=(gpointer)ptr->data;
									menuitem=newMenuItem(MAINTOOLSBLANKTOOL,submenu);
								}
							ptr=g_list_next(ptr);
						}
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
//paste
					menuitem=newImageMenuItem(CONTEXTPASTE,tabMenu);
					contextMenus[CONTEXTPASTE]->page=page;
					contextMenus[CONTEXTPASTE]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTPASTE]);
//toggle show hidden
					if(showHidden==false)
						{
							menuitem=newImageMenuItem(CONTEXTSHOW,tabMenu);
							contextMenus[CONTEXTSHOW]->page=page;
							contextMenus[CONTEXTSHOW]->treepath=treepath;
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTSHOW]);
						}
					else
						{
							menuitem=newImageMenuItem(CONTEXTHIDE,tabMenu);
							contextMenus[CONTEXTHIDE]->page=page;
							contextMenus[CONTEXTHIDE]->treepath=treepath;
							g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTHIDE]);
						}
//new bm
					menuitem=newImageMenuItem(CONTEXTBMNEW,tabMenu);
					contextMenus[CONTEXTBMNEW]->page=page;
					contextMenus[CONTEXTBMNEW]->treepath=treepath;
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(contextMenuActivate),(void*)contextMenus[CONTEXTBMNEW]);
				}
			gtk_menu_popup(GTK_MENU(tabMenu),NULL,NULL,NULL,NULL,event->button,event->time);
			gtk_widget_show_all((GtkWidget*)tabMenu);
			//printf("11111111111111\n");
			//thing=true;
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
	gchar			*path;
	GtkTreeIter		iter;
	gboolean		isdir;
	char			*command;

	gtk_tree_model_get_iter(GTK_TREE_MODEL(page->listStore),&iter,tree_path);
	gtk_tree_model_get(GTK_TREE_MODEL(page->listStore),&iter,FILEPATH,&path,ISDIR,&isdir,-1);
	if(isdir==true)
		{
			setCurrentFolderForTab(path,page,true,false);
			clearForward(page);
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
			setCurrentFolderForTab(path,page,true,false);
			clearForward(page);
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

//printf("path=%s, mountpoint=%s\n",path,mountpoint);

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
	setCurrentFolderForTab(mountpoint,page,true,false);
	clearForward(page);

	free(path);
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
	fileName=NULL;
	validName=false;

 	if(((long)ptr==-1) && (gtk_entry_get_text_length((GtkEntry*)askentryText[ENTERFILENAMETXT])>0))
 		{
			fileName=strdup(gtk_entry_get_text((GtkEntry*)askentryText[ENTERFILENAMETXT]));
			validName=true;
		}
	gtk_widget_destroy(askentryWindow);
}

void doAbout(GtkWidget *widget,gpointer data)
{
	const char	copyright[]=COPYRITE " \n" MYEMAIL;
	const char	*aboutboxstring=ABOUTBOXSTRING;
	char		*licence;

	g_file_get_contents(GDOCSFOLDER "/gpl-3.0.txt",&licence,NULL,NULL);

	gtk_show_about_dialog((GtkWindow*)mainWindow,"authors",authors,"comments",aboutboxstring,"copyright",copyright,"version",VERSION,"website",MYWEBSITE,"website-label","KKFileManager Homepage","program-name","KKFileManager","logo-icon-name",ABOUTICON,"license",licence,NULL);

	free(licence);
}

void externalTool(GtkWidget *widget,gpointer data)
{
//printf("void externalTool(GtkWidget *widget,gpointer data)\n");
	toolStruct		*tool=(toolStruct*)data;
	char			*tooldirname=NULL;
	char			*strarray=NULL;
	unsigned int	buffersize=1000;
	char			*itemarray=NULL;
	pageStruct		*page=NULL;
	GList			*pages=pageList;
//printf("tool->menuName=%s\ntool->filePath=%s\n",tool->menuName,tool->filePath);
//printf("tool->command=%s\ntool->flags=%i\n",tool->command,tool->flags);
//printf("tool->inTerminal=%i\ntool->inPopUp=%i\n",tool->inTerminal,tool->inPopUp);
//printf("tool->alwaysPopup=%i\ntool->clearView=%i\n",tool->alwaysPopup,tool->clearView);
//printf("tool->comment=%s\ntool->global=%i\n",tool->comment,tool->global);
//printf("tool->runAsRoot=%i\ntool->keyCode=%i\n",tool->runAsRoot,tool->keyCode);
//printf("tool->useBar=%i\n",tool->useBar);


	strarray=(char*)calloc(buffersize,1);
	itemarray=(char*)calloc(buffersize,1);

	while(pages!=NULL)
		{
			page=(pageStruct*)pages->data;
			if(page!=NULL)
				{
					if(buffersize<(strlen(strarray)+strlen(page->thisFolder)+2))
						{
							buffersize+=1000;
							strarray=(char*)realloc(strarray,buffersize);
						}
					strcat(strarray,page->thisFolder);
					strcat(strarray,"\n");
				}
			pages=pages->next;
		}
	itemarray=selectionToString("\n");
	if(strarray!=NULL)
		setenv("KKFILEMANAGER_TAB_LIST",strarray,1);
	if(itemarray!=NULL)
		setenv("KKFILEMANAGER_SELECTED_ITEMS",itemarray,1);
	if(page!=NULL)
		setenv("KKFILEMANAGER_CURRENT_TAB",page->thisFolder,1);

	tooldirname=g_path_get_dirname(tool->filePath);
	sinkReturn=chdir(tooldirname);
	system(tool->command);

	if(strarray!=NULL)
		free(strarray);
	unsetenv("KKFILEMANAGER_TAB_LIST");
	if(itemarray!=NULL)
		free(itemarray);
	unsetenv("KKFILEMANAGER_SELECTED_ITEMS");
	unsetenv("KKFILEMANAGER_CURRENT_TAB");
}

void setFileProps(GtkWidget* widget,gpointer ptr)
{
	char		*command;
	unsigned	mode;
	const char	*recursive="";

	if((long)ptr<0)
		{
			if((long)ptr==DIALOGAPPLY)
				{
					//file modes
					mode=((gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[READ1CHK])*4 + gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[WRITE2CHK])*2 + gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[EXECUTE3CHK])*1)*0100 ) + 
					((gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[READ4CHK])*4 + gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[WRITE5CHK])*2 + gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[EXECUTE6CHK])*1)*010 ) + 
					((gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[READ7CHK])*4 + gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[WRITE8CHK])*2 + gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[EXECUTE9CHK])*1)*01 );
					//world modes
					mode=mode+(gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[SUID10CHK])*4 + gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[SGID11CHK])*2 + gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[STICKY12CHK])*1)*01000;
					
					command=(char*)alloca(PATH_MAX);
					if(gtk_toggle_button_get_active((GtkToggleButton*)filepropsCheck[RECURSIVE13CHK])==true)
						recursive="-R";
					sprintf(command,"chown %s %s:%s \"%s\"",recursive,gtk_entry_get_text((GtkEntry*)filepropsText[TXT0]),gtk_entry_get_text((GtkEntry*)filepropsText[TXT1]),filePath);
					system(command);					
					sprintf(command,"chmod %s 00%o \"%s\"",recursive,mode,filePath);
					system(command);
				}			
			gtk_widget_destroy((GtkWidget*)filepropsWindow);
		}
}

