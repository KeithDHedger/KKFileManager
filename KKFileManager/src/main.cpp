/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>

enum {NEWVBOX=0,NEWHBOX};
enum {TEXT_COLUMN,PIXBUF_COLUMN,BUTTON_NUM};

GtkWidget		*window = NULL;
GtkWidget		*scrollBox=NULL;
GtkIconView		*iconView=NULL;
GtkListStore	*listStore=NULL;
GtkWidget		*mainVBox=NULL;
char			*sinkReturnStr;
char			*thisFolder;

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

void setNewPixbuf(GdkPixbuf *pixbuf,int bnum,const char *type)
{
	GtkTreeIter iter;

	gtk_list_store_append(listStore, &iter);
	if(pixbuf!=NULL)
		gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pixbuf,TEXT_COLUMN,type,BUTTON_NUM,bnum,-1);
}

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

	asprintf(&command,"find %s -maxdepth 1 -mindepth 1 -type d -not -path '*/\\.*'|sort",thisFolder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						buffer[strlen(buffer)-1]=0;
					pixbuf=getPixBuf(buffer);
					setNewPixbuf(pixbuf,cnt,basename(buffer));
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
					setNewPixbuf(pixbuf,cnt,basename(buffer));
					cnt++;
				}
			pclose(fp);
		}
	free(command);
}

void doIconView(void)
{
	listStore=gtk_list_store_new(3,G_TYPE_STRING,GDK_TYPE_PIXBUF,G_TYPE_INT);
	iconView=(GtkIconView*)gtk_icon_view_new();
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconView),PIXBUF_COLUMN);
	gtk_icon_view_set_text_column (GTK_ICON_VIEW (iconView),TEXT_COLUMN);
	gtk_icon_view_set_model(GTK_ICON_VIEW(iconView),GTK_TREE_MODEL(listStore));
	gtk_icon_view_set_item_padding(GTK_ICON_VIEW(iconView),0);
	
	populateStore();

	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(iconView),GTK_SELECTION_SINGLE);
	gtk_icon_view_set_item_orientation(GTK_ICON_VIEW(iconView),GTK_ORIENTATION_VERTICAL);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(iconView),-1);
	gtk_icon_view_set_reorderable(GTK_ICON_VIEW(iconView),false);

	gtk_icon_view_set_item_width ((GtkIconView *)iconView,96);
}

int main(int argc,char **argv)
{

	GtkWidget	*vbox;

//	thisFolder=strdup(getenv("HOME"));
	thisFolder=strdup("/media/LinuxData/Development64/Projects/KKFileManager");
	gtk_init(&argc,&argv);

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

	gtk_main();
}