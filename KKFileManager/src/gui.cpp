/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:16:28 GMT 2016 kdhedger68713@gmail.com

 * This file (gui.cpp) is part of KKFileManager.

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
#include <gdk/gdk.h>

#include "globals.h"
GdkPixbuf *pixbuft;

void shutdown(GtkWidget* widget,gpointer data)
{
	gtk_main_quit();
}

GtkWidget *createNewBox(int orient,bool homog,int spacing)
{
	GtkWidget	*retwidg=NULL;

#ifdef _USEGTK3_
	if(orient==NEWVBOX)
		retwidg=gtk_box_new(GTK_ORIENTATION_VERTICAL,spacing);
	else
		retwidg=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,spacing);
	gtk_box_set_homogeneous((GtkBox*)retwidg,homog);
#else
	if(orient==NEWVBOX)
		retwidg=gtk_vbox_new(homog,spacing);
	else
		retwidg=gtk_hbox_new(homog,spacing);
#endif

	return(retwidg);
}

void switchPage(GtkNotebook *notebook,gpointer arg1,guint arg2,gpointer user_data)
{

	GtkWidget	*widg;
	widg=gtk_notebook_get_nth_page(notebook,arg2);

	pageStruct	*page=getPageStructByIDFromList((unsigned)(long)g_object_get_data((GObject*)widg,"pageid"));
	if(page!=NULL)
		gtk_entry_set_text(locationTextBox,page->thisFolder);
}

void setNewPagePixbuf(GdkPixbuf *pixbuf,const char *type,const char *path,bool isdir,pageStruct *page)
{
	GtkTreeIter iter;

	gtk_list_store_append(page->listStore, &iter);
	if(pixbuf!=NULL)
		gtk_list_store_set(page->listStore,&iter,PIXBUF_COLUMN,pixbuf,TEXT_COLUMN,type,FILEPATH,path,ISDIR,isdir,-1);
}


char *getMimeType(const char *path)
{
	char	*retdata;
	gboolean	uncertain=false;

	retdata=g_content_type_guess(path,NULL,0,&uncertain);
//printf("g_content_type_guess=%s\n",retdata);
	if(uncertain==false)
		return(retdata);
	
	retdata=strdup(magic_file(magicInstance,path));
//printf("magic_file=%s\n",retdata);
	return(retdata);
}

unsigned hashMimeType(char* cp)
{
	unsigned hash=5381;
	while(*cp)
		hash=33*hash^*cp++;
	return(hash);
}

GdkPixbuf* getPixBuf(const char *file)
{
	GIcon			*icon=NULL;
	GtkIconInfo		*info=NULL;
	GdkPixbuf		*pb=NULL;
	char			*mime;
	unsigned		hash;
	bool			issymlink=false;
	char			*symlink=NULL;
	char			*newf=NULL;
	bool			isbrokenlink=false;

	mime=getMimeType(file);
//printf("mime=%s file %s\n",mime,file);
	if(g_file_test(file,G_FILE_TEST_IS_SYMLINK)==true)
		{
			issymlink=true;
			free(mime);
			newf=realpath(file,NULL);
			if(newf==NULL)
				{
					mime=strdup("application-octet-stream");
					hash=hashMimeType(mime);
					isbrokenlink=true;
				}
			else
				{
					mime=getMimeType(newf);
					symlink=strdup(mime);
					symlink=strncpy(symlink,"sym",3);
					hash=hashMimeType(symlink);
				}
		}
	else
		hash=hashMimeType(mime);

		if(symlink!=NULL)
			free(symlink);
		if(newf!=NULL)
			free(newf);

	if(pixBuffCache.find(hash)!=pixBuffCache.end())
		{
			free(mime);
			return(pixBuffCache.find(hash)->second);
		}

	icon=g_content_type_get_icon(mime);
	info=gtk_icon_theme_lookup_by_gicon(defaultTheme,icon,48,(GtkIconLookupFlags)0);
	if(info==NULL)
		{
			icon=g_content_type_get_icon("text-x-generic");
			info=gtk_icon_theme_lookup_by_gicon(defaultTheme,icon,48,(GtkIconLookupFlags)0);
			if(info==NULL)
				{
					icon=g_content_type_get_icon("text-x-generic");
					info=gtk_icon_theme_lookup_by_gicon(gnomeTheme,icon,48,(GtkIconLookupFlags)0);
				}
		}
	else
		{
			icon=g_content_type_get_icon(mime);
		}

	pb=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,48,NULL);

	if(issymlink==true)
		gdk_pixbuf_composite(symLink,pb,48-16,48-16,16,16,48-16,48-16,1.0,1.0,GDK_INTERP_NEAREST,255);

	if(isbrokenlink==true)
		gdk_pixbuf_composite(brokenLink,pb,0,48-16,16,16,0,48-16,1.0,1.0,GDK_INTERP_NEAREST,255);

	pixBuffCache[hash]=pb;
	free(mime);
	return(pb);
}

void populatePageStore(pageStruct *page)
{
	GdkPixbuf		*pixbuf;

	char			*command;
	FILE			*fp=NULL;
	char			buffer[2048];
	int				cnt=0;
	int				upcnt=0;

	//gtk_widget_freeze_child_notify((GtkWidget*)page->iconView);
	gtk_list_store_clear(page->listStore);
	asprintf(&command,"find \"%s\" -maxdepth 1 -mindepth 1 -type d -follow -not -path '*/\\.*'|sort",page->thisFolder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						buffer[strlen(buffer)-1]=0;
					pixbuf=getPixBuf(buffer);
					setNewPagePixbuf(pixbuf,basename(buffer),buffer,true,page);
					cnt++;
				}
			pclose(fp);
		}
	free(command);
	asprintf(&command,"find \"%s\" -maxdepth 1 -mindepth 1 -not -type d -follow -not -path '*/\\.*'|sort",page->thisFolder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						buffer[strlen(buffer)-1]=0;
					pixbuf=getPixBuf(buffer);
					setNewPagePixbuf(pixbuf,basename(buffer),buffer,false,page);
					upcnt++;
					if(upcnt>20)
						{
							//gtk_widget_thaw_child_notify((GtkWidget*)iconView);
							gtk_main_iteration_do (false);
							//gtk_widget_freeze_child_notify((GtkWidget*)iconView);
							upcnt=0;
						}
					cnt++;
				}
			pclose(fp);
		}
	free(command);
	gtk_widget_show_all((GtkWidget*)page->scrollBox);
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

void newIconView(pageStruct *page)
{
	page->listStore=gtk_list_store_new(NUMCOLS,G_TYPE_STRING,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_BOOLEAN);
	page->iconView=(GtkIconView*)gtk_icon_view_new();
	
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(page->iconView),PIXBUF_COLUMN);
	gtk_icon_view_set_text_column(GTK_ICON_VIEW(page->iconView),TEXT_COLUMN);
	gtk_icon_view_set_model(GTK_ICON_VIEW(page->iconView),GTK_TREE_MODEL(page->listStore));
	gtk_icon_view_set_item_padding(GTK_ICON_VIEW(page->iconView),0);
	g_signal_connect (page->iconView,"item-activated",G_CALLBACK(selectItem),page);	
	g_signal_connect (page->iconView,"button-press-event",G_CALLBACK(buttonDown),page);	
	populatePageStore(page);

	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(page->iconView),GTK_SELECTION_SINGLE);
	gtk_icon_view_set_item_orientation(GTK_ICON_VIEW(page->iconView),GTK_ORIENTATION_VERTICAL);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(page->iconView),-1);
	gtk_icon_view_set_reorderable(GTK_ICON_VIEW(page->iconView),true);

	gtk_icon_view_set_item_width(page->iconView,iconSize);
	gtk_icon_view_set_column_spacing(page->iconView,iconPadding);
}

void buidMainGui(const char *startdir)
{
	//pixbuft=gdk_pixbuf_new_from_file_at_size("/media/LinuxData/Development64/Projects/KKFileManager/KKFileManager/resources/pixmaps/KKFileManager.png",-1,48,NULL);

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);

	mainNotebook=(GtkNotebook*)gtk_notebook_new();
	g_signal_connect(G_OBJECT(mainNotebook),"switch-page",G_CALLBACK(switchPage),NULL);

	addNewPage((char*)startdir);
	mainVBox=createNewBox(NEWVBOX,false,0);

	toolBar=(GtkToolbar*)gtk_toolbar_new();
	setUpToolBar();
	gtk_box_pack_start(GTK_BOX(mainVBox),(GtkWidget*)toolBar,false,false,0);
	gtk_box_pack_start(GTK_BOX(mainVBox),(GtkWidget*)mainNotebook,true,true,0);

	gtk_container_add((GtkContainer*)window,mainVBox);
	gtk_entry_set_text(locationTextBox,startdir);
	gtk_widget_show_all(window);
}

