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
	toolBarLayout=strdup("OUBFH");

	gtk_init(&argc,&argv);

	magicInstance=magic_open(MAGIC_MIME_TYPE);
	magic_load(magicInstance,NULL);

	buidMainGui();

	gtk_main();
	magic_close(magicInstance);
}