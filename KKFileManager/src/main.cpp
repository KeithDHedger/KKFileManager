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
enum {PIXBUF_COLUMN,TEXT_COLUMN,BUTTON_NUM};

GtkWidget		*window = NULL;
GtkIconView		*iconView=NULL;
GtkListStore	*listStore=NULL;
GtkWidget		*mainVBox=NULL;
char			*sinkReturnStr;

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

//void setNewPixbuf(const char *path,int bnum,const char *type)
void setNewPixbuf(GdkPixbuf *pixbuf,int bnum,const char *type)
{
	GtkTreeIter iter;
	//GdkPixbuf	*pbuf;

	gtk_list_store_append(listStore, &iter);
	//pbuf=gdk_pixbuf_new_from_file(path,NULL);
	if(pixbuf!=NULL)
		{
			gtk_list_store_set(listStore,&iter,PIXBUF_COLUMN,pixbuf,TEXT_COLUMN,type,BUTTON_NUM,bnum,-1);
			//g_object_unref(pbuf);
		}
//	gtk_widget_set_sensitive((GtkWidget*)tool[bnum],false);

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
		free(retstr);
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
//	asprintf(&command,"file  -b --mime-type \"%s\"|sed 's@/@-@g;s@-httpd@@g;s@program@executable@g;s@msdos@ms-dos@g'",path);
asprintf(&command,"file  -b --mime-type \"%s\"",path);
retdata=oneLiner(command);
return(retdata);	
//}
	if(strchr(path,'.')!=NULL)
		{
			char *dot=strchr((char*)path,'.');
			asprintf(&command,"cat /usr/share/mime/globs|sed -n '/\\%s$/p'|awk -F \":\" '{print $1}'|awk -F \"/\" '{print $2}'",dot);
			retdata=oneLiner(command);
			if(strlen(retdata)>4)
				{
					printf("------%s<<<\n",command);
					return(retdata);
				}
		}

//	if(strchr(path,'.')==NULL)
		{
			asprintf(&command,"file  -b --mime-type \"%s\"|awk -F \"/\" '{print $2}'",path);
			retdata=oneLiner(command);
		}
//	else
//		{
//			char *dot=strchr((char*)path,'.');
//			asprintf(&command,"cat /usr/share/mime/globs|sed -n '/\\%s$/p'|awk -F \":\" '{print $1}'",dot);
//			retdata=oneLiner(command);
//		}
	printf(">>>%s<<<\n",command);
	return(retdata);	
}

GdkPixbuf* getPixBuf(const char *file)
{
	GtkIconTheme	*theme=gtk_icon_theme_get_default();
	GdkPixbuf		*pb=NULL;
	char			*mime=getMimeType(file);
	char			buffer[2048];

GIcon *icon=g_content_type_get_icon (mime);
printf(">>>%s<<<\n",g_icon_to_string (icon));
////g_content_type_get_icon
////gtk_icon_theme_choose_icon
////g_loadable_icon_load
//printf("name=%s\n",g_content_type_get_generic_icon_name ("text/x-shellscript"));
//printf("name=%s\n",g_content_type_from_mime_type ("text/x-shellscript"));
theme=gtk_icon_theme_get_default();
GtkIconInfo *info=gtk_icon_theme_lookup_by_gicon(theme,icon,48,(GtkIconLookupFlags)0);
printf(">>>%s<<<\n",gtk_icon_info_get_filename (info));
	//pb=gtk_icon_theme_load_icon(theme,buffer,48,(GtkIconLookupFlags)0,NULL);
	//if(pb!=NULL)
	//	{
	pb=gdk_pixbuf_new_from_file_at_size  (gtk_icon_info_get_filename (info),-1,48,NULL);
			return(pb);

#if 0
	printf("file=%s,raw mimetype=%s
",file,mime);

	buffer[0]=0;
	sprintf(buffer,"inode-%s",mime);
//	printf("buffer=%s
",buffer);
	pb=gtk_icon_theme_load_icon(theme,buffer,48,(GtkIconLookupFlags)0,NULL);
	if(pb!=NULL)
		{
			return(pb);
		}
	buffer[0]=0;
	sprintf(buffer,"gnome-%s",mime);
	//printf("buffer=%s
",buffer);
	pb=gtk_icon_theme_load_icon(theme,buffer,48,(GtkIconLookupFlags)0,NULL);
	if(pb!=NULL)
		{
			return(pb);
		}
	buffer[0]=0;
	sprintf(buffer,"application-%s",mime);
	//printf("buffer=%s
",buffer);
	pb=gtk_icon_theme_load_icon(theme,buffer,48,(GtkIconLookupFlags)0,NULL);
	if(pb!=NULL)
		{
			return(pb);
		}
	buffer[0]=0;
	sprintf(buffer,"gnome-mime-application-%s",mime);
	//printf("buffer=%s
",buffer);
	pb=gtk_icon_theme_load_icon(theme,buffer,48,(GtkIconLookupFlags)0,NULL);
	if(pb!=NULL)
		{
			return(pb);
		}
	buffer[0]=0;
	sprintf(buffer,"gnome-mime-text-%s",mime);
	//printf("buffer=%s
",buffer);
	pb=gtk_icon_theme_load_icon(theme,buffer,48,(GtkIconLookupFlags)0,NULL);
	if(pb!=NULL)
		{
			return(pb);
		}
	buffer[0]=0;
	sprintf(buffer,"text-%s",mime);
	pb=gtk_icon_theme_load_icon(theme,buffer,48,(GtkIconLookupFlags)0,NULL);
	if(pb!=NULL)
		{
			return(pb);
		}
	buffer[0]=0;
	sprintf(buffer,"audio-%s",mime);
	pb=gtk_icon_theme_load_icon(theme,buffer,48,(GtkIconLookupFlags)0,NULL);
	if(pb!=NULL)
		{
			return(pb);
		}

#endif
}

//content_type_get_icon
void populateStore(void)
{
	char			*type;
	GtkIconTheme	*theme=gtk_icon_theme_get_default();
	GdkPixbuf		*pixbuf;
	char			*mime=NULL;
	const char		*data[]={"/media/LinuxData/Development64/Projects/KKEdit/kkedit_fr_FR.po","/media/LinuxData/Development64/Projects/KKEdit/config.status","/media/LinuxData/Development64/Projects/KKEdit","/bin/bash","/media/LinuxData/Development64/Projects/KKEdit/KKEdit/resources/src/kkeditmsg.mpz","/media/Music/Mp3s/Music/Abba/Gold/01 Dancing Queen.mp3","/media/Music/Flacs/Music/Abba/Gold/01 Dancing Queen.flac","/media/LinuxData/Development64/Projects/KKFileManager/KKFileManager/src/main.cpp",NULL};
	int j=0;
	while(data[j]!=NULL)
//	for(int j=0;j<5;j++)
		{
			pixbuf=getPixBuf(data[j]);
//			mime=getMimeType(data[j]);
//			printf("mime=%s\n",mime);
//			if(gtk_icon_theme_has_icon(theme,mime))
//				{
//			printf("got mime=%s\n",mime);
//					pixbuf=gtk_icon_theme_load_icon(theme,mime,48,(GtkIconLookupFlags)0,NULL);

//					setNewPixbuf(pixbuf,j,"a");
//				}


//pixbuf=gtk_icon_theme_load_icon (theme,"gnome-directory",48,(GtkIconLookupFlags)0,NULL);
					setNewPixbuf(pixbuf,j,"a");
//			mime="inode-directory";
//			if(gtk_icon_theme_has_icon(theme,mime))
//				{
//				printf("xxxxxx\n");
//					pixbuf=gtk_icon_theme_load_icon(theme,mime,48,(GtkIconLookupFlags)0,NULL);
//					setNewPixbuf(pixbuf,j,"a");
//				}
					
			//setNewPixbuf("/usr/share/icons/gnome/48x48/places/folder.png",j,"a");
#if 0
			type=strndup((char*)&toolBarLayout[j],1);
			switch(toolBarLayout[j])
				{
					case 'N':
//new
						setNewPixbuf(PREFSNEW,ITEMNEW,type);
						break;
					case 'O':
//open+recent
						setNewPixbuf(PREFSOPEN,ITEMOPEN,type);
						break;
					case 'S':
//save
						setNewPixbuf(PREFSSAVE,ITEMSAVE,type);
						break;

					case 'X':
//cut
						setNewPixbuf(PREFSCUT,ITEMCUT,type);
						break;
					case 'C':
//copy
						setNewPixbuf(PREFSCOPY,ITEMCOPY,type);
						break;
					case 'P':
//paste
						setNewPixbuf(PREFSPASTE,ITEMPASTE,type);
						break;
					case 'U':
//undo
						setNewPixbuf(PREFSUNDO,ITEMUNDO,type);
						break;
					case 'R':
//redo
						setNewPixbuf(PREFSREDO,ITEMREDO,type);
						break;
					case 'F':
//find
						setNewPixbuf(PREFSFIND,ITEMFIND,type);
						break;
					case 'G':
//navigation
						setNewPixbuf(PREFSQUESTION,ITEMGOTO,type);
						break;
//go back
					case 'B':
						setNewPixbuf(PREFSBACK,ITEMBACK,type);
						break;
//go forward
					case 'W':
						setNewPixbuf(PREFSFORWARD,ITEMFORWARD,type);
						break;
//goto line num
					case '9':
						setNewPixbuf(PREFSLINENUM,ITEMLINE,type);
						break;
//find api
					case 'A':
						setNewPixbuf(PREFSAPI,ITEMAPI,type);
						break;
//find qt api
					case 'Q':
						setNewPixbuf(PREFSQTAPI,ITEMQTAPI,type);
						break;
					case 'D':
//find define
						setNewPixbuf(PREFSFINDDEF,ITEMDEFINE,type);
						break;
					case 'L':
//live search
						setNewPixbuf(PREFSLIVE,ITEMLIVE,type);
						break;
//seperator
					case 's':
						setNewPixbuf(PREFSSEPERATOR,ITEMSEPERATOR,type);
						gtk_widget_set_sensitive((GtkWidget*)tool[14],true);
					break;
//expander
					case 'E':
						setNewPixbuf(PREFSEXPAND,ITEMEXPANDER,type);
						gtk_widget_set_sensitive((GtkWidget*)tool[15],true);
						break;
				}
			free(&type);

#endif
			j++;
		}
}

void doIconView(void)
{
	GtkCellRenderer *renderer;

	listStore=gtk_list_store_new(3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_INT);
	iconView=(GtkIconView*)gtk_icon_view_new();
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconView),PIXBUF_COLUMN);
	gtk_icon_view_set_model(GTK_ICON_VIEW(iconView),GTK_TREE_MODEL(listStore));
	gtk_icon_view_set_item_padding(GTK_ICON_VIEW(iconView),0);
	
	populateStore();

	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(iconView),GTK_SELECTION_SINGLE);
	gtk_icon_view_set_item_orientation(GTK_ICON_VIEW(iconView),GTK_ORIENTATION_HORIZONTAL);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(iconView),-1);
	gtk_icon_view_set_reorderable(GTK_ICON_VIEW(iconView),false);

	renderer=gtk_cell_renderer_pixbuf_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(iconView),renderer,false);

	renderer=gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(iconView),renderer,false);

	gtk_box_pack_start(GTK_BOX(mainVBox),(GtkWidget*)iconView,true,true,2);
	//g_signal_connect(G_OBJECT(iconView),"button-press-event",G_CALLBACK(clickIt),NULL);
}


int main(int argc,char **argv)
{

	gtk_init(&argc,&argv);
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(shutdown),NULL);

	mainVBox=createNewBox(NEWVBOX,false,0);
	doIconView();
	gtk_container_add((GtkContainer*)window,mainVBox);
	gtk_widget_show_all(window);


GIcon *icon=g_content_type_get_icon ("application/x-executable");
printf(">>>%s<<<\n",g_icon_to_string (icon));
////g_content_type_get_icon
////gtk_icon_theme_choose_icon
////g_loadable_icon_load
//printf("name=%s\n",g_content_type_get_generic_icon_name ("text/x-shellscript"));
//printf("name=%s\n",g_content_type_from_mime_type ("text/x-shellscript"));
GtkIconTheme	*theme=gtk_icon_theme_get_default();
GtkIconInfo *info=gtk_icon_theme_lookup_by_gicon(theme,icon,48,(GtkIconLookupFlags)0);
printf(">>>%s<<<\n",gtk_icon_info_get_filename (info));
	gtk_main();
}