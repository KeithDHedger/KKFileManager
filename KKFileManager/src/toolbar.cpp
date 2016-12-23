/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:12:20 GMT 2016 kdhedger68713@gmail.com

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
	char		*command;
	pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());

	if(g_file_test(gtk_entry_get_text(entry),G_FILE_TEST_IS_DIR)==false)
		{
			asprintf(&command,"mimeopen -L -n \"%s\" &",gtk_entry_get_text(entry));
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
			asprintf(&command,"find  \"%s\"* -mindepth 0 -maxdepth 0 -follow -type d -print",text);
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
						break;
				}
		}

}