/*
 *
 * ©K. D. Hedger. Wed 30 Nov 14:16:37 GMT 2016 kdhedger68713@gmail.com

 * This file (dialogs.h) is part of KKFileManager.

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

#include <gtk/gtk.h>

#ifndef _DIALOGS_
#define _DIALOGS_

enum {ENTERFILENAMETXT=0};

extern char 		*fileName;
extern GtkWidget	*askentryText[];
extern GtkWidget	*askentryWindow;
extern bool			validName;

void doAskForFilename(GtkWidget* widget,gpointer data);
void setAskEntry(GtkWidget* widget,gpointer ptr);

#endif
