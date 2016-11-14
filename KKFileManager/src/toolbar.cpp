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

#include "globals.h"

void goUp(GtkWidget *widget,gpointer data)
{
	char	*hold;

	hold=g_path_get_dirname(thisFolder);
	thisFolder=hold;
	populateStore();
}

void goHome(GtkWidget *widget,gpointer data)
{
	free(thisFolder);
	thisFolder=strdup((char*)g_get_home_dir());
	populateStore();
}

void getLocation(GtkEntry *entry,GdkEvent *event,gpointer data)
//void goLocation(GtkWidget *widget,gpointer data)
{
	const char	*text=gtk_entry_get_text(entry);
	if(text!=NULL && strlen(text)>0)
		{
//			free(thisFolder);
//			thisFolder=strdup((char*)text);
//			populateStore();
			printf(">>>%s<<<\n",text);
		}
}

void goLocation(GtkEntry *entry,GdkEvent *event,gpointer data)
//void goLocation(GtkWidget *widget,gpointer data)
{
	const char	*text=gtk_entry_get_text(entry);
	if(text!=NULL && strlen(text)>0)
		{
			free(thisFolder);
			thisFolder=strdup((char*)text);
			populateStore();
		}
}


void setUpToolBar(void)
{
	GtkToolItem		*toolbutton;
//#ifdef _USEGTK3_
//	GtkWidget		*image;
//#endif
	GtkWidget		*menu;

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
						g_signal_connect(G_OBJECT(locationTextBox),"activate",G_CALLBACK(goLocation),locationTextBox);
						break;
//new tab
					case 'O':
						break;
//go home
					case 'H':
						homeButton=gtk_tool_button_new_from_stock(GTK_STOCK_HOME);
						gtk_toolbar_insert(toolBar,homeButton,-1);
						g_signal_connect(G_OBJECT(homeButton),"clicked",G_CALLBACK(goHome),NULL);
						break;
//go up
					case 'U':
						upButton=gtk_tool_button_new_from_stock(GTK_STOCK_GO_UP);
						gtk_toolbar_insert(toolBar,upButton,-1);
						g_signal_connect(G_OBJECT(upButton),"clicked",G_CALLBACK(goUp),NULL);
						break;
//go back
					case 'B':
//						{
//#ifdef _USEGTK3_
//						image=gtk_image_new_from_icon_name(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_LARGE_TOOLBAR);
//						backButton=gtk_menu_tool_button_new(image,BACK_TOOLBAR_LABEL);
//#else
						backButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
//#endif

						gtk_toolbar_insert(toolBar,backButton,-1);
#if 0
						//g_signal_connect(G_OBJECT(backButton),"clicked",G_CALLBACK(navigateHistory),(void*)NAVLAST);
						//gtk_widget_set_tooltip_text((GtkWidget*)backButton,BACK_TT_LABEL);
//back history
						menu=gtk_menu_new();
						gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(backButton),menu);
						gtk_menu_tool_button_set_arrow_tooltip_text(GTK_MENU_TOOL_BUTTON(backButton),BACK_MENU_TT_LABEL);

						globalHistory->setHistBackMenu(menu);		

#endif
						break;
//go forward
					case 'F':
//#ifdef _USEGTK3_
//						image=gtk_image_new_from_icon_name(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_LARGE_TOOLBAR);
//						forwardButton=gtk_menu_tool_button_new(image,FORWARD_TOOLBAR_LABEL);
//#else
						forwardButton=gtk_menu_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
//#endif
						gtk_toolbar_insert(toolBar,forwardButton,-1);
//						g_signal_connect(G_OBJECT(forwardButton),"clicked",G_CALLBACK(navigateHistory),(void*)NAVNEXT);
//						gtk_widget_set_tooltip_text((GtkWidget*)forwardButton,FORWARD_TT_LABEL);
#if 0
//foward history
						menu=gtk_menu_new();
						gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(forwardButton),menu);
						gtk_menu_tool_button_set_arrow_tooltip_text(GTK_MENU_TOOL_BUTTON(forwardButton),FORWARD_MENU_TT_LABEL);

						globalHistory->setHistForwardMenu(menu);		

#endif
						break;
				}
		}

}