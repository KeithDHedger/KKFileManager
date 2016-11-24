/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:13:28 GMT 2016 kdhedger68713@gmail.com

 * This file (globals.cpp) is part of KKFileManager.

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
#include <magic.h>
#include <map>

#include "globals.h"

//main app
GtkToolbar		*toolBar=NULL;
GtkWidget		*mainWindow = NULL;
GtkWidget		*mainVBox=NULL;
char			*sinkReturnStr=NULL;
magic_t			magicInstance=NULL;
GtkIconTheme	*defaultTheme=NULL;
GtkIconTheme	*gnomeTheme=NULL;
GtkNotebook		*mainNotebook=NULL;
GList			*pageList=NULL;
GtkAccelGroup	*accgroup=NULL;
GtkWidget		*mainHPane=NULL;
GtkWidget		*leftVBox=NULL;
GtkWidget		*leftVPane=NULL;
GtkListStore	*diskList=NULL;
GtkTreeView		*diskView=NULL;
GtkWidget		*bookmarkList=NULL;
GtkWidget		*menuBar=NULL;

//file menu
GtkWidget		*fileMenu=NULL;
GtkWidget		*menuItemNew=NULL;

//tool bar
GtkToolItem		*upButton=NULL;
GtkToolItem		*backButton=NULL;
GtkToolItem		*forwardButton=NULL;
GtkToolItem		*homeButton=NULL;
GtkToolItem		*newButton=NULL;
GtkToolItem		*locationButton=NULL;
GtkEntry		*locationTextBox=NULL;

//mimes and stuff
std::map<unsigned,GdkPixbuf*>	pixBuffCache;
GdkPixbuf		*symLink=NULL;
GdkPixbuf		*brokenLink=NULL;
GFile			*dirPath=NULL;
GFileMonitor	*monitorDir=NULL;

//tabs
unsigned		pageCnt=1000;
#ifdef _USEGTK3_
unsigned		iconSize=48;
unsigned		iconPadding=16;
#else
unsigned		iconSize=96;
unsigned		iconPadding=16;
#endif
unsigned		maxTabChars=32;

//menus

//prefs
bool			showMenuIcons=true;
char			*toolBarLayout=NULL;
char			*diskIncludePattern;
char			*diskExcludePattern;

//dand
unsigned		fromPageID=0;

//odds
int				sinkReturn;

//global functions
char* oneLiner(const char *command)
{
	FILE	*fp=NULL;
	char	*retstr=(char*)calloc(1,256);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			sinkReturnStr=fgets(retstr,256,fp);
			pclose(fp);
		}
	if(sinkReturnStr==NULL)
		{
			free(retstr);
			return(NULL);
		}
	else
		{
			if(retstr[strlen(retstr)-1]=='\n')
				retstr[strlen(retstr)-1]=0;
		}
	return(retstr);
}

void setCurrentFolderForTab(const char *newfolder,pageStruct *page)
{
	GtkEntryCompletion	*completion;
	GtkListStore		*list;

	if(g_file_test(newfolder,G_FILE_TEST_IS_DIR)==false)
		return;

	if(strcmp(page->thisFolder,newfolder)==0)
		return;
	if(newfolder!=NULL && strlen(newfolder)>0)
		{
			free(page->thisFolder);
			page->thisFolder=strdup((char*)newfolder);
			gtk_entry_set_text(locationTextBox,page->thisFolder);
			gtk_editable_set_position((GtkEditable*)locationTextBox,-1);
			populatePageStore(page);
			monitorFolderForPage(page);
			completion=gtk_entry_get_completion(locationTextBox);
			list=(GtkListStore*)gtk_entry_completion_get_model(completion);
        	gtk_list_store_clear(list);
			gtk_entry_completion_complete(completion);
			gtk_widget_show_all(page->vBox);
			updateTabLabel(page);
		}
}

int yesNo(const char *question,char *file)
{
	GtkWidget	*dialog;
	int			result;

	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,"%s %s",question,file);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_YES,GTK_RESPONSE_YES,GTK_STOCK_NO,GTK_RESPONSE_CANCEL,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"What Do You Want To Do?");
	result=gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	return(result);
}

char *getUniqueFilename(const char *path)
{
	char		buffer[PATH_MAX+20];
	unsigned	cnt=0;

	sprintf(buffer,"%s",path);
	if(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
		{
			cnt=0;
			while(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
				{
					cnt++;
					sprintf(buffer,"%s %u",path,cnt);
				}
		}
	else
		{
			sprintf(buffer,"%s",path);
		}
	return(strdup(buffer));

}

