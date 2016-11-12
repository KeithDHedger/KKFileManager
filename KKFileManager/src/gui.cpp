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

#include "globals.h"

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

void setNewPixbuf(GdkPixbuf *pixbuf,const char *type,const char *path,bool isdir)
{
	GtkTreeIter iter;

	gtk_list_store_append(listStore, &iter);
	if(pixbuf!=NULL)
		gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pixbuf,TEXT_COLUMN,type,FILEPATH,path,ISDIR,isdir,-1);
}

char *getMimeType(const char *path)
{
	char *retdata;
	char *command;

	if(strchr(path,'.')!=NULL)
		{
			char *dot=strchr((char*)path,'.');
			asprintf(&command,"cat /usr/share/mime/globs|sed -n '/\\%s$/p'|awk -F \":\" '{print $1}'",dot);
			retdata=oneLiner(command);
			if(retdata!=NULL)
			{
			if(strlen(retdata)>4)
				{
					//printf("command=%s returns:%s\n",command,retdata);
					return(retdata);
				}
			}
		}

	asprintf(&command,"file  -b --mime-type \"%s\"",path);
//	printf("command=%s",command);
	retdata=oneLiner(command);
//	printf(", returns:%s\n",retdata);
	return(retdata);	
}

GdkPixbuf* getPixBuf(const char *file)
{
	GIcon			*icon=NULL;
	GtkIconInfo		*info=NULL;
	GdkPixbuf		*pb=NULL;
	GtkIconTheme	*theme=gtk_icon_theme_get_default();
	GtkIconTheme	*gnometheme=gtk_icon_theme_new();
	char			*mime=getMimeType(file);
//	printf("file=%s\n",file);
	theme=gtk_icon_theme_get_default();
	icon=g_content_type_get_icon(mime);
	info=gtk_icon_theme_lookup_by_gicon(theme,icon,48,(GtkIconLookupFlags)0);
	if(info==NULL)
		{
			icon=g_content_type_get_icon("text-x-generic");
			info=gtk_icon_theme_lookup_by_gicon(theme,icon,48,(GtkIconLookupFlags)0);
			if(info==NULL)
				{
					gtk_icon_theme_set_custom_theme(gnometheme,"gnome");
					icon=g_content_type_get_icon("text-x-generic");
					info=gtk_icon_theme_lookup_by_gicon(gnometheme,icon,48,(GtkIconLookupFlags)0);
				}
		}
	else
		{
			icon=g_content_type_get_icon(mime);
		}
//	printf("info filename=%s\n\n",gtk_icon_info_get_filename (info));
	pb=gdk_pixbuf_new_from_file_at_size (gtk_icon_info_get_filename(info),-1,48,NULL);
	g_object_unref(gnometheme);
	return(pb);
}

void populateStore(void)
{
	GdkPixbuf		*pixbuf;

	char			*command;
	FILE			*fp=NULL;
	char			buffer[2048];
	int				cnt=0;

	gtk_list_store_clear(listStore);
	asprintf(&command,"find %s -maxdepth 1 -mindepth 1 -type d -not -path '*/\\.*'|sort",thisFolder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						buffer[strlen(buffer)-1]=0;
					pixbuf=getPixBuf(buffer);
					setNewPixbuf(pixbuf,basename(buffer),buffer,true);
					cnt++;
				}
			pclose(fp);
		}
	free(command);
	asprintf(&command,"find %s -maxdepth 1 -mindepth 1 -not -type d -not -path '*/\\.*'|sort",thisFolder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						buffer[strlen(buffer)-1]=0;
					pixbuf=getPixBuf(buffer);
					setNewPixbuf(pixbuf,basename(buffer),buffer,false);
					cnt++;
				}
			pclose(fp);
		}
	free(command);
}

void selectItem(GtkIconView *icon_view,GtkTreePath *tree_path,gpointer user_data)
{
	printf("clicked\n");
	GtkListStore	*store;
	gchar			*path;
	GtkTreeIter		iter;
	gboolean		isdir;
	char			*command;

	store=GTK_LIST_STORE(user_data);

	gtk_tree_model_get_iter(GTK_TREE_MODEL(store),&iter,tree_path);
	gtk_tree_model_get(GTK_TREE_MODEL(store),&iter,FILEPATH,&path,ISDIR,&isdir,-1);
	printf("path=%s\n",path);
	printf("---%i\n",isdir);
	if(isdir==true)
		{
			free(thisFolder);
			thisFolder=strdup(path);
			populateStore();
		}
	else
		{
			asprintf(&command,"mimeopen -L -n \"%s\" &",path);
			printf("command=%s\n",command);
			system(command);
			free(command);
		}
	free(path);
}

void doIconView(void)
{
	listStore=gtk_list_store_new(NUMCOLS,G_TYPE_STRING,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_BOOLEAN);
	iconView=(GtkIconView*)gtk_icon_view_new();
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconView),PIXBUF_COLUMN);
	gtk_icon_view_set_text_column(GTK_ICON_VIEW (iconView),TEXT_COLUMN);
	gtk_icon_view_set_model(GTK_ICON_VIEW(iconView),GTK_TREE_MODEL(listStore));
	gtk_icon_view_set_item_padding(GTK_ICON_VIEW(iconView),0);
	g_signal_connect (iconView,"item-activated",G_CALLBACK(selectItem),listStore);	
	populateStore();

	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(iconView),GTK_SELECTION_SINGLE);
	gtk_icon_view_set_item_orientation(GTK_ICON_VIEW(iconView),GTK_ORIENTATION_VERTICAL);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(iconView),-1);
	gtk_icon_view_set_reorderable(GTK_ICON_VIEW(iconView),false);

	gtk_icon_view_set_item_width ((GtkIconView *)iconView,96);
}

void buidMainGui(void)
{
	GtkWidget	*vbox;

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);

	scrollBox=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)scrollBox,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	mainVBox=createNewBox(NEWVBOX,false,0);
	vbox=createNewBox(NEWVBOX,false,0);
	doIconView();
	gtk_container_add((GtkContainer*)window,mainVBox);
	gtk_container_add((GtkContainer*)scrollBox,(GtkWidget*)iconView);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)scrollBox,true,true,0);
	gtk_box_pack_start(GTK_BOX(mainVBox),(GtkWidget*)vbox,true,true,0);

	gtk_widget_show_all(window);

}

