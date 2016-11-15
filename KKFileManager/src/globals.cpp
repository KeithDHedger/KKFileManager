/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:13:28 GMT 2016 kdhedger68713@gmail.com

 * This file (globals.cpp) is part of KKFileManager.

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
#include <magic.h>
#include <map>

#include "globals.h"

//main app
GtkToolbar		*toolBar=NULL;
GtkWidget		*window = NULL;
GtkWidget		*scrollBox=NULL;
GtkIconView		*iconView=NULL;
GtkListStore	*listStore=NULL;
GtkWidget		*mainVBox=NULL;
char			*sinkReturnStr=NULL;
char			*thisFolder=NULL;
magic_t			magicInstance=NULL;
char			*toolBarLayout=NULL;
GtkIconTheme	*defaultTheme=NULL;
GtkIconTheme	*gnomeTheme=NULL;

//tool bar
GtkToolItem		*upButton=NULL;
GtkToolItem		*backButton=NULL;
GtkToolItem		*forwardButton=NULL;
GtkToolItem		*homeButton=NULL;
GtkToolItem		*locationButton=NULL;
GtkEntry		*locationTextBox=NULL;

//mimes and stuff
std::map<unsigned,GdkPixbuf*>	pixBuffCache;

//tabs

//global functions
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

