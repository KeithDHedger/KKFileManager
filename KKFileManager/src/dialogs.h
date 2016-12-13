/*
 *
 * ©K. D. Hedger. Mon  5 Dec 13:19:52 GMT 2016 kdhedger68713@gmail.com

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

#define _ISDIALOG_
extern char			*fileName;
extern bool			validName;

extern GtkWidget	*askentryText[];
extern GtkWidget	*askentryWindow;

void doAskForFilename(const char* filename);

#endif

