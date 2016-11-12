/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 */

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <gtk/gtk.h>

#include "globals.h"
#include "gui.h"

int main(int argc,char **argv)
{


	thisFolder=strdup("/media/LinuxData/Development64/Projects/KKFileManager");
	gtk_init(&argc,&argv);

	buidMainGui();

	gtk_main();
}