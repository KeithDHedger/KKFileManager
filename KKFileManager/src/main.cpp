/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 */

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <gtk/gtk.h>
#include <map>

#include "globals.h"
#include "gui.h"

int			cnt=0;
const char	*mimetypestocache[]=
{
		"application/x-executable",
		"application/x-maneditdoc",
		"inode/directory",
		"inode/x-empty",
		"text/plain",
		"text/x-c++src",
		"text/x-gettext-translation",
		"text/x-patch",
		"text/x-python",
		"text/x-shellscript",
		NULL
};

void themeChanged(GtkIconTheme *icon_theme,gpointer user_data)
{
	GtkWidget	*widg;
	unsigned	pageid;
	pageStruct	*page;

	pixBuffCache.clear();
	for(int j=0;j<gtk_notebook_get_n_pages(mainNotebook);j++)
		{
			widg=gtk_notebook_get_nth_page(mainNotebook,j);
			pageid=(unsigned)(long)g_object_get_data((GObject*)widg,"pageid");
			page=getPageStructByIDFromList(pageid);
			populatePageStore(page);
		}
}

gboolean loadCache(gpointer data)
{
	unsigned		hash;
	GIcon			*icon=NULL;
	GtkIconInfo		*info=NULL;
	GdkPixbuf		*pb=NULL;

	if(mimetypestocache[cnt]==NULL)
		return(FALSE);

	hash=hashMimeType((char*)mimetypestocache[cnt]);
	if(pixBuffCache.find(hash)!=pixBuffCache.end())
		{
			cnt++;
			return(true);
		}

	icon=g_content_type_get_icon(mimetypestocache[cnt]);
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
			icon=g_content_type_get_icon(mimetypestocache[cnt]);
		}

	pb=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,48,NULL);
	pixBuffCache[hash]=pb;
	cnt++;
	return(true);
}

void loadPrefs(void)
{
	char	*filename;

	sinkReturn=asprintf(&filename,"%s/%s",getenv("HOME"),APPFOLDENAME);
	g_mkdir_with_parents(filename,493);
	free(filename);
	sinkReturn=asprintf(&filename,"%s/%s/kkfiemanager.rc",getenv("HOME"),APPFOLDENAME);

	loadVarsFromFile(filename,kkfilemanager_rc);
	if(windowAllocData!=NULL)
		sscanf(windowAllocData,"%i %i %i %i",(int*)&windowWidth,(int*)&windowHeight,(int*)&windowX,(int*)&windowY);

	free(filename);
}

int main(int argc,char **argv)
{
	GtkIconInfo	*info;
	const char	*startdir;
	char		*origpath=NULL;

	if(argc>1)
		{
			origpath=realpath(argv[1],NULL);
			if(origpath!=NULL)
				startdir=origpath;
			else
				startdir=(char*)"/";
		}
	else
		startdir=(char*)"/";
	toolBarLayout=strdup("NUBFHL");
	diskIncludePattern=strdup("*sd?*");
	diskExcludePattern=strdup("*");

	gtk_init(&argc,&argv);
	defaultTheme=gtk_icon_theme_get_default();
	gnomeTheme=gtk_icon_theme_new();
	gtk_icon_theme_set_custom_theme(gnomeTheme,"gnome");

	info=gtk_icon_theme_lookup_icon(gnomeTheme,"emblem-symbolic-link",16,(GtkIconLookupFlags)0);
	symLink=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,16,NULL);
#ifdef _USEGTK3_
	g_object_unref(info);
#else
	gtk_icon_info_free(info);
#endif
	info=gtk_icon_theme_lookup_icon(gnomeTheme,"emblem-unreadable",16,(GtkIconLookupFlags)0);
	brokenLink=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,16,NULL);
#ifdef _USEGTK3_
	g_object_unref(info);
#else
	gtk_icon_info_free(info);
#endif

	magicInstance=magic_open(MAGIC_MIME_TYPE);
	magic_load(magicInstance,NULL);
	g_timeout_add (50,loadCache,NULL);

	loadPrefs();

	buidMainGui(startdir);

	free(origpath);


	g_signal_connect_after(G_OBJECT(defaultTheme),"changed",G_CALLBACK(themeChanged),NULL);
	gtk_main();

	g_object_unref(defaultTheme);
	magic_close(magicInstance);
}