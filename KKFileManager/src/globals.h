/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:12:50 GMT 2016 kdhedger68713@gmail.com

 * This file (globals.h) is part of KKFileManager.

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

#ifndef _GLOBALS_
#define _GLOBALS_

#include <gtk/gtk.h>
#include <magic.h>

#include "toolbar.h"
#include "gui.h"

enum {NEWVBOX=0,NEWHBOX};
enum {TEXT_COLUMN,PIXBUF_COLUMN,FILEPATH,ISDIR,NUMCOLS};

//main app
extern GtkToolbar	*toolBar;
extern GtkWidget	*window;
extern GtkWidget	*scrollBox;
extern GtkIconView	*iconView;
extern GtkListStore	*listStore;
extern GtkWidget	*mainVBox;
extern char			*sinkReturnStr;
extern char			*thisFolder;
extern magic_t		magicInstance;
extern char			*toolBarLayout;
extern GtkWidget	*toolBarBox;

//tool bar
extern GtkToolItem		*upButton;
extern GtkToolItem		*backButton;
extern GtkToolItem		*forwardButton;

//global functions
char* oneLiner(const char *command);

#endif
