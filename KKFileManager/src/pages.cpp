/*
 *
 * ©K. D. Hedger. Wed 16 Nov 15:08:37 GMT 2016 keithdhedger@gmail.com

 * This file (pages.cpp) is part of KKFileManager.

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

#include "globals.h"

unsigned getPageIdFromTabNum(int tabnum)
{
	GtkWidget	*widg;
	widg=gtk_notebook_get_nth_page(mainNotebook,tabnum);

	if(widg==NULL)
		return(0);
	else
		return((unsigned)(long)g_object_get_data((GObject*)widg,"pageid"));
}

unsigned getPageIdFromTab(void)
{
	GtkWidget	*widg;
	widg=gtk_notebook_get_nth_page(mainNotebook,gtk_notebook_get_current_page(mainNotebook));

	if(widg==NULL)
		return(0);
	else
		return((unsigned)(long)g_object_get_data((GObject*)widg,"pageid"));
}

pageStruct *getPageStructByIDFromList(unsigned pageid)
{
	pageStruct	*page=NULL;
	GList		*list;

	list=pageList;
	while(list!=NULL)
		{
			page=(pageStruct*)list->data;
			if((page!=NULL) && (page->pageID==pageid))
				return(page);
			list=list->next;
		}			
	return(NULL);
}

pageStruct *getPageFromCurrentTab(void)
{
	return(getPageStructByIDFromList(getPageIdFromTab()));
}

void addNewPage(char *startdir)
{
	pageStruct	*page;
	char		*basename;

	page=(pageStruct*)calloc(1,sizeof(pageStruct));
	page->scrollBox=(GtkScrolledWindow*)gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(page->scrollBox,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	if(g_file_test(startdir,G_FILE_TEST_IS_DIR)==true)
		page->thisFolder=strdup(startdir);
	else
		page->thisFolder=strdup(getenv("HOME"));
	page->pageID=pageCnt;
	basename=g_path_get_basename(page->thisFolder);
	page->tabBox=makeNewTab(basename,page);
	page->toggleOff=false;
	page->startedDrag=false;
	page->stdBehaviour=false;
	page->fList=NULL;
	page->bList=NULL;
	page->searchPath=0;
	pageList=g_list_prepend(pageList,(gpointer)page);

//set_property
	newIconView(page);
	monitorFolderForPage(page);

	page->vBox=createNewBox(NEWVBOX,true,0);
	gtk_container_add((GtkContainer*)page->scrollBox,(GtkWidget*)page->iconView);
	gtk_box_pack_start(GTK_BOX(page->vBox),(GtkWidget*)page->scrollBox,true,true,0);

	gtk_widget_show_all((GtkWidget*)page->vBox);
	gtk_notebook_append_page(mainNotebook,page->vBox,page->tabBox);
	gtk_notebook_set_tab_reorderable(mainNotebook,page->vBox,true);

	gtk_container_child_set_property((GtkContainer*)mainNotebook,page->vBox,"tab-expand",&boolVal);
	g_object_set_data(G_OBJECT(page->vBox),"pageid",(gpointer)(long)page->pageID);
	gtk_widget_show_all((GtkWidget*)mainNotebook);
	pageCnt++;
	gtk_entry_set_text(locationTextBox,page->thisFolder);
	gtk_editable_set_position((GtkEditable*)locationTextBox,-1);

	gtk_notebook_set_current_page(mainNotebook,gtk_notebook_get_n_pages(mainNotebook)-1);
	free(basename);
}

void monitorFolderForPage(pageStruct *page)
{
	if(page!=NULL)
		{
			if(G_IS_OBJECT(page->dirPath))
				g_object_unref(page->dirPath);
			if(G_IS_OBJECT(page->monitorDir))
				g_object_unref(page->monitorDir);
			page->dirPath=g_file_new_for_path(page->thisFolder);
			//page->monitorDir=g_file_monitor_directory(page->dirPath,(GFileMonitorFlags)(G_FILE_MONITOR_NONE),NULL,NULL);
			page->monitorDir=g_file_monitor_directory(page->dirPath,(GFileMonitorFlags)(G_FILE_MONITOR_SEND_MOVED|G_FILE_MONITOR_WATCH_MOUNTS|G_FILE_MONITOR_WATCH_MOVES),NULL,NULL);
			g_signal_connect(G_OBJECT(page->monitorDir),"changed",G_CALLBACK(dirChanged),page);
		}
}

void flushFolderBuffer(pageStruct *page)
{
	if(page==NULL)
		return;
	for(int j=0;j<page->fileCnt;j++)
		free(page->fileList[j]);
	free(page->fileList);
}



