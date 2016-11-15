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

int main(int argc,char **argv)
{


	if(argc>1)
		thisFolder=strdup(argv[1]);
	else
		thisFolder=strdup("/");
	toolBarLayout=strdup("OUBFHL");

	gtk_init(&argc,&argv);
	defaultTheme=gtk_icon_theme_get_default();
	gnomeTheme=gtk_icon_theme_new();
	gtk_icon_theme_set_custom_theme(gnomeTheme,"gnome");

	magicInstance=magic_open(MAGIC_MIME_TYPE);
	magic_load(magicInstance,NULL);

	buidMainGui();

	gtk_main();

	g_object_unref(defaultTheme);
	magic_close(magicInstance);
}