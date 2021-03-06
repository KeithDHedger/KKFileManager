/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:12:20 GMT 2016 keithdhedger@gmail.com

 * This file (toolbar.cpp) is part of KKFileManager.

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
#include <string.h>
#include <sys/stat.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <sys/types.h>
#include <pwd.h>

#include "globals.h"

GtkToolItem *createNewToolItem(const char *stock,const char *label)
{
	GtkToolItem	*button;
#ifdef _USEGTK3_
	GtkWidget	*image;
	image=gtk_image_new_from_icon_name(stock,GTK_ICON_SIZE_LARGE_TOOLBAR);
	gtk_image_set_pixel_size((GtkImage*)image,24);
	button=gtk_tool_button_new(image,label);
#else
	button=gtk_tool_button_new_from_stock(stock);
	gtk_tool_button_set_label((GtkToolButton*)button,label);
#endif

	return(button);
}

void goUp(GtkWidget *widget,gpointer data)
{
	char		*hold;
	pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());

	hold=g_path_get_dirname(page->thisFolder);
	setCurrentFolderForTab(hold,page,true,false);
	free(hold);
}

void refreshView(GtkWidget *widget,gpointer data)
{
	pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());

	setCurrentFolderForTab(page->thisFolder,page,true,true);
}


void goHome(GtkWidget *widget,gpointer data)
{
	pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());
	setCurrentFolderForTab(g_get_home_dir(),page,true,false);
}

void goNew(GtkWidget *widget,gpointer data)
{
	pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());
	if(page!=NULL)
		addNewPage(page->thisFolder);
	else
		addNewPage((char*)getenv("HOME"));
}

void goLocation(GtkEntry *entry,GdkEvent *event,gpointer data)
{
	char				*command;
	struct passwd		*pws;
	pageStruct			*page=getPageStructByIDFromList(getPageIdFromTab());
	const char			*text=gtk_entry_get_text(entry);
	const char			*mounttypes[]={"smb://","ftp://","ssh://","http://","dav://","/dev/",NULL};
	unsigned			cnt=0;
	networkDriveStruct	*nm=(networkDriveStruct*)alloca(sizeof(networkDriveStruct));
	char				portname[16];
	char				password[128];
	char				uid[128];
	char				sharepath[NAME_MAX+1];
	bool				isdev=false;
	char				mbuff[1024];

	nm->url=NULL;
	nm->fstype=NULL;
	nm->host=NULL;
	nm->port=NULL;
	nm->user=NULL;
	nm->pass=NULL;
	nm->path=NULL;

	while(mounttypes[cnt]!=NULL)
		{
			if(g_str_has_prefix(text,mounttypes[cnt])==true)
				break;
			cnt++;
		}
//smb://keithhedger:hogandnana@192.168.1.66:445/lansite
//smb://guest@192.168.1.201/sdcard
	if(cnt<6)
		{
			parseNetworkUrl(text,nm);
			//printDriveDetails(nm);
			updateNetHistoryFile(text,0);

			switch(cnt)
				{
					case 0:
					//smb://keithhedger:hogandnana@192.168.1.66/media
					//udevil mount -t cifs  -o username=keithhedger,password=hogandnana,uid=1000,port=445 //192.168.1.66/lansite
						if(nm->user==NULL)
							nm->user=strdup("guest");

						if(nm->port!=NULL)
							sprintf(portname,",port=%s",nm->port);
						else
							sprintf(portname,"%s","");

						if(nm->pass!=NULL)
							sprintf(password,",password=%s",nm->pass);
						else
							sprintf(password,"%s","");

						pws=getpwnam(nm->user);
						if(pws!=NULL)
							sprintf(uid,",uid=%i",pws->pw_uid);
						else
							sprintf(uid,"%s","");
			
						asprintf(&command,"udevil mount -t cifs -o username=%s%s%s%s //%s%s",nm->user,password,uid,portname,nm->host,nm->path);
						//printf(">>>command=%s<<<\n",command);
						system(command);
						free(command);
						break;
					case 1:
					//ftp://kdhedger:sparky@192.168.1.201
					//udevil mount ftp://kdhedger:sparky@192.168.1.201
					//ftp://speedtest.tele2.net
					{
						const char	*at="";
						if(nm->user!=NULL)
							at="@";
						else
							nm->user=strdup("");
						if(nm->pass!=NULL)
							{
								sprintf(password,":%s",nm->pass);
//								colon=":";
							}
						else
							sprintf(password,"%s","");
						
						asprintf(&command,"udevil mount ftp://%s%s%s%s",nm->user,password,at,nm->host);
						//printf(">>>command=%s<<<\n",command);
						system(command);
						free(command);
					}
						break;
					case 2:
						{
						//ssh://192.168.1.66
							if(nm->path!=NULL)
								sprintf(sharepath,"%s",nm->path);
							else
								sprintf(sharepath,"%s","/");

							asprintf(&command,"mkdir -vp ~/.local/%s/%s",nm->host,sharepath);
							system(command);
							free(command);
							asprintf(&command,"sshfs %s:%s ~/.local/%s/%s",nm->host,sharepath,nm->host,sharepath);
							system(command);
							//printf("ssh -> %s\n",command);
							free(command);
						}
						break;
					case 3:
					case 4:
						//davfs://user:pass@server
						//http://kdhedger:sparky@192.168.1.201:8080
						//http://192.168.1.201
						if(nm->user!=NULL)
							sprintf(uid,"%s",nm->user);
						else
							sprintf(uid,"%s","");
						if(nm->pass!=NULL)
							sprintf(password,"%s",nm->pass);
						else
							sprintf(password,"%s","");

						if(nm->port!=NULL)
							sprintf(portname,":%s",nm->port);
						else
							sprintf(portname,":%s","8080");

						asprintf(&command,"echo -e \"%s\\n%s\\n\"|udevil mount http://%s%s",uid,password,nm->host,portname);
						system(command);
						//printf("dav->%s\n",command);
						free(command);
						break;
					case 5:
						isdev=true;
						asprintf(&command,"udevil mount %s",text);
						system(command);
						free(command);
						break;
				}

			if(isdev==true)
				snprintf(mbuff,1023,"mount |grep %s|awk '{print $3}'",text);
			else
				snprintf(mbuff,1023,"mount |grep %s|grep %s|awk '{print $3}'",nm->path,nm->host);

			sinkReturnStr=oneLiner(mbuff,(char*)&mbuff);

			if(strlen(mbuff)>0)
				setCurrentFolderForTab(mbuff,page,true,true);
			return;
		}

	if(g_file_test(gtk_entry_get_text(entry),G_FILE_TEST_IS_DIR)==false)
		{
			if(text[0]=='/')
				asprintf(&command,"mimeopen -L -n \"%s\" &",text);
			
			if(text[0]=='$')
				asprintf(&command,"%s &",&text[1]);

			system(command);
			free(command);
			return;
		}
	setCurrentFolderForTab(gtk_entry_get_text(entry),page,true,true);
}

gboolean trapTabKey(GtkEntry *entry,GdkEvent *event,gpointer data)
{
	if(event->key.keyval==GDK_KEY_Tab)
		{
			gtk_editable_set_position((GtkEditable *)entry,-1);
			return(true);
		}
	return(false);
}

gboolean getLocation(GtkEntry *entry,GdkEvent *event,gpointer data)
{
	char			*command;
	FILE			*fp=NULL;
	char			buffer[2048];
    GtkListStore	*list;
    GtkTreeIter		iter;
	GtkEntryCompletion	*completion;

	if(event->key.keyval==GDK_KEY_Up)
		return(true);
	if(event->key.keyval==GDK_KEY_Down)
		return(true);
	if(event->key.keyval==GDK_KEY_Return)
		{
			goLocation(entry,NULL,NULL);
			return(true);
		}

	const char	*text=gtk_entry_get_text(entry);
	if(text!=NULL && strlen(text)>0)
		{
			asprintf(&command,"find  \"%s\"* -mindepth 0 -maxdepth 0 -follow -type d -print 2>/dev/null",text);
			fp=popen(command,"r");
			if(fp!=NULL)
				{
					completion=gtk_entry_get_completion(entry);
					list=(GtkListStore*)gtk_entry_completion_get_model(completion);
        			gtk_list_store_clear(list);
					while(fgets(buffer,2048,fp))
						{
							if(strlen(buffer)>0)
								buffer[strlen(buffer)-1]=0;
							gtk_list_store_append(list,&iter);
							gtk_list_store_set(list,&iter,0,buffer,-1);
						}
					gtk_entry_completion_complete(completion);
				}
			free(command);
			pclose(fp);
		}
	return(false);
}

void setUpToolBar(void)
{
	GtkEntryCompletion	*completion;
	GtkListStore		*store;
	GtkWidget			*menu;
#ifdef _USEGTK3_
	GtkWidget			*image;
#endif
 
	for(int j=0;j<(int)strlen(toolBarLayout);j++)
		{
			switch(toolBarLayout[j])
				{
//location
					case 'L':
						locationTextBox=(GtkEntry*)gtk_entry_new();
						locationButton=gtk_tool_item_new();
						gtk_container_add((GtkContainer *)locationButton,(GtkWidget*)locationTextBox);
						gtk_tool_item_set_expand(locationButton,true);
						gtk_toolbar_insert(toolBar,locationButton,-1);
						g_signal_connect(G_OBJECT(locationTextBox),"key-release-event",G_CALLBACK(getLocation),locationTextBox);
						g_signal_connect(G_OBJECT(locationTextBox),"key-press-event",G_CALLBACK(trapTabKey),locationTextBox);
						g_signal_connect(G_OBJECT(locationTextBox),"button-press-event",G_CALLBACK(locateBarHistory),locationTextBox);
						/* Create the completion object */
						completion=gtk_entry_completion_new();
						gtk_entry_completion_set_inline_completion(completion,true);
						gtk_entry_completion_set_popup_single_match(completion,false);
						gtk_entry_set_completion(locationTextBox,completion);
						g_object_unref(completion);
						store=gtk_list_store_new(1,G_TYPE_STRING);
						gtk_entry_completion_set_model(completion,(GtkTreeModel *)store);
						g_object_unref(store);
						gtk_entry_completion_set_text_column(completion,0);
						break;
//new tab
					case 'N':
						newButton=createNewToolItem(GTK_STOCK_NEW,"New");
						gtk_toolbar_insert(toolBar,newButton,-1);
						g_signal_connect(G_OBJECT(newButton),"clicked",G_CALLBACK(goNew),NULL);
						break;
//go home
					case 'H':
						homeButton=createNewToolItem(GTK_STOCK_HOME,"Home");
						gtk_toolbar_insert(toolBar,homeButton,-1);
						g_signal_connect(G_OBJECT(homeButton),"clicked",G_CALLBACK(goHome),NULL);
						break;
//go up
					case 'U':
						upButton=createNewToolItem(GTK_STOCK_GO_UP,"Up");
						gtk_toolbar_insert(toolBar,upButton,-1);
						g_signal_connect(G_OBJECT(upButton),"clicked",G_CALLBACK(goUp),NULL);
						break;
//refresh
					case 'R':
						refreshButton=createNewToolItem(GTK_STOCK_REFRESH,"Refresh");
						gtk_toolbar_insert(toolBar,refreshButton,-1);
						g_signal_connect(G_OBJECT(refreshButton),"clicked",G_CALLBACK(refreshView),NULL);
						break;

//connect
					case 'C':
#ifdef _USEGTK3_
						image=gtk_image_new_from_icon_name(GTK_STOCK_NETWORK,GTK_ICON_SIZE_LARGE_TOOLBAR);
						gtk_image_set_pixel_size((GtkImage*)image,24);
						connectButton=gtk_menu_tool_button_new(image,"Connect");
#else
						connectButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_NETWORK);
#endif
						gtk_toolbar_insert(toolBar,connectButton,-1);
						g_signal_connect(G_OBJECT(connectButton),"clicked",G_CALLBACK(doConnectWrap),NULL);
//connect history
						menu=gtk_menu_new();
						gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(connectButton),menu);
						g_signal_connect(G_OBJECT(connectButton),"show-menu",G_CALLBACK(connectMenu),NULL);
						g_signal_connect_after(GTK_MENU_SHELL(menu),"selection-done",G_CALLBACK(clearMenu),NULL);
						break;					
//go back
					case 'B':
#ifdef _USEGTK3_
						image=gtk_image_new_from_icon_name(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_LARGE_TOOLBAR);
						gtk_image_set_pixel_size((GtkImage*)image,24);
						backButton=gtk_menu_tool_button_new(image,"Back");
#else
						backButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
#endif
						gtk_toolbar_insert(toolBar,backButton,-1);
						g_signal_connect(G_OBJECT(backButton),"clicked",G_CALLBACK(navigateHistory),NULL);
//back history
						menu=gtk_menu_new();
						gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(backButton),menu);
						g_signal_connect(G_OBJECT(backButton),"show-menu",G_CALLBACK(backMenu),NULL);
						g_signal_connect_after(GTK_MENU_SHELL(menu),"selection-done",G_CALLBACK(clearMenu),NULL);
						break;

//go forward
					case 'F':
#ifdef _USEGTK3_
						image=gtk_image_new_from_icon_name(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_LARGE_TOOLBAR);
						gtk_image_set_pixel_size((GtkImage*)image,24);
						forwardButton=gtk_menu_tool_button_new(image,"Forward");
#else
						forwardButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
#endif
						gtk_toolbar_insert(toolBar,forwardButton,-1);
						g_signal_connect(G_OBJECT(forwardButton),"clicked",G_CALLBACK(navigateHistory),(void*)1);
//foward history
						menu=gtk_menu_new();
						gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(forwardButton),menu);
						g_signal_connect(G_OBJECT(forwardButton),"show-menu",G_CALLBACK(backMenu),(void*)1);
						g_signal_connect_after(GTK_MENU_SHELL(menu),"selection-done",G_CALLBACK(clearMenu),NULL);
						break;
				}
		}

}