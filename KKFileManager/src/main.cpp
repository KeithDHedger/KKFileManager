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
	pixBuffCache.clear();
	populateStore();
}

int main(int argc,char **argv)
{
	GtkIconInfo	*info;

	if(argc>1)
		thisFolder=(char*)argv[1];
	else
		thisFolder=(char*)"/";
	toolBarLayout=strdup("OUBFHL");

	gtk_init(&argc,&argv);
	defaultTheme=gtk_icon_theme_get_default();
	gnomeTheme=gtk_icon_theme_new();
	gtk_icon_theme_set_custom_theme(gnomeTheme,"gnome");

	info=gtk_icon_theme_lookup_icon(gnomeTheme,"emblem-symbolic-link",16,(GtkIconLookupFlags)0);
	symLink=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,16,NULL);
	gtk_icon_info_free(info);
	info=gtk_icon_theme_lookup_icon(gnomeTheme,"emblem-unreadable",16,(GtkIconLookupFlags)0);
	brokenLink=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,16,NULL);
	gtk_icon_info_free(info);

	magicInstance=magic_open(MAGIC_MIME_TYPE);
	magic_load(magicInstance,NULL);

	chdir(thisFolder);
	thisFolder=get_current_dir_name();
	buidMainGui();

	dirPath=g_file_new_for_path(thisFolder);
	monitorDir=g_file_monitor_directory(dirPath,(GFileMonitorFlags)G_FILE_MONITOR_SEND_MOVED,NULL,NULL);
	g_signal_connect(G_OBJECT(monitorDir),"changed",G_CALLBACK(dirChanged),NULL);

	g_signal_connect_after(G_OBJECT(defaultTheme),"changed",G_CALLBACK(themeChanged),NULL);
	gtk_main();

	g_object_unref(defaultTheme);
	magic_close(magicInstance);
}