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

	buidMainGui(startdir);

	free(origpath);

	g_signal_connect_after(G_OBJECT(defaultTheme),"changed",G_CALLBACK(themeChanged),NULL);
	gtk_main();

	g_object_unref(defaultTheme);
	magic_close(magicInstance);
}