/*
 *
 * ©K. D. Hedger. Fri 23 Dec 11:50:14 GMT 2016 kdhedger68713@gmail.com

 * This file (history.cpp) is part of KKFileManager.

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

#include "globals.h"

void doGoBack(GtkWidget *widget,GList *lst)
{
	GList		*blst=NULL;
	pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());

	setCurrentFolderForTab((char*)lst->data,page,true,false);
	blst=g_list_last(page->bList);
	while(blst!=lst)
		{
			addToHistory(page,(char*)blst->data,false);
			if(blst!=NULL)
				{
					page->bList=g_list_remove_link(page->bList,blst);
					free((char*)blst->data);
					g_list_free_1(blst);
					blst=g_list_last(page->bList);
				}
		}

	page->bList=g_list_remove_link(page->bList,lst);
	free((char*)lst->data);
	g_list_free_1(lst);
}

void doGoForward(GtkWidget *widget,GList *lst)
{
	GList		*flst=NULL;
	pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());

	setCurrentFolderForTab((char*)lst->data,page,true,false);
	flst=page->fList;
	while(flst!=lst)
		{
			addToHistory(page,(char*)flst->data,true);
			if(flst!=NULL)
				{
					page->fList=g_list_remove_link(page->fList,flst);
					free((char*)flst->data);
					g_list_free_1(flst);
					flst=page->fList;
				}
		}
	page->fList=g_list_remove_link(page->fList,lst);
	free((char*)lst->data);
	g_list_free_1(lst);
}

void navigateHistory(GtkToolButton *toolbutton,gpointer data)
{
	GList		*flst=NULL;
	GList		*blst=NULL;
	pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());

	if(page==NULL)
		return;

	if(data==NULL)
		{
			if(page->bList!=NULL)
				{
					addToHistory(page,page->thisFolder,false);
					blst=g_list_last(page->bList);
					setCurrentFolderForTab((char*)blst->data,page,false,false);
					page->bList=g_list_remove_link(page->bList,blst);
					free((char*)blst->data);
					g_list_free_1(blst);
				}
		}
	else
		{
			if(page->fList!=NULL)
				{
					addToHistory(page,page->thisFolder,true);
					flst=page->fList;
					setCurrentFolderForTab((char*)flst->data,page,false,false);
					page->fList=g_list_remove_link(page->fList,flst);
					free((char*)flst->data);
					g_list_free_1(flst);
				}
		}
}

void backMenu(GtkMenuToolButton *toolbutton,gpointer data)
{
	GtkWidget	*menu;
	GtkWidget	*menuitem;
	GList		*lst;
	pageStruct	*page=getPageStructByIDFromList(getPageIdFromTab());

	if(page==NULL)
		return;

	menu=gtk_menu_tool_button_get_menu(toolbutton);
	if(data==NULL)
		{
			lst=page->bList;
			while(lst!=NULL)
				{
					menuitem=gtk_menu_item_new_with_label(strdup((char*)lst->data));
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doGoBack),lst);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					lst=lst->next;
				}
		}
	else
		{
			lst=page->fList;
			while(lst!=NULL)
				{
					menuitem=gtk_menu_item_new_with_label((char*)lst->data);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doGoForward),lst);
					lst=lst->next;
				}
		}
						//	g_signal_connect_after(G_OBJECT(menu),"activate-item",G_CALLBACK(doGoBack3),lst);
	gtk_widget_show_all(menu);
}

void freeHistoryList(gpointer data)
{
	free(data);
}

void addToHistory(pageStruct *page,char *folder,bool addtoback)
{
	if(page==NULL)
		return;

	if(addtoback==true)
		page->bList=g_list_append(page->bList,strdup(folder));
	else
		page->fList=g_list_prepend(page->fList,strdup(folder));
}

void clearForward(pageStruct *page)
{
	g_list_free_full(page->fList,freeHistoryList);
	page->fList=NULL;
}

void clearMenu(GtkMenuShell *menushell,gpointer user_data)
{
	GList		*childs;
	childs=gtk_container_get_children ((GtkContainer*)menushell);
	for(unsigned j=0;j<g_list_length(childs);j++)
		gtk_widget_destroy((GtkWidget*)g_list_nth_data(childs,j));

	g_list_free(childs);
}





