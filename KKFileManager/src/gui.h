/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:16:35 GMT 2016 kdhedger68713@gmail.com

 * This file (gui.h) is part of KKFileManager.

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

#ifndef _GUI_
#define _GUI_

#include "pages.h"

GtkWidget *createNewBox(int orient,bool homog,int spacing);
void buidMainGui(const char *startdir);
//void populateStore(void);
void populatePageStore(pageStruct *page);
void newIconView(pageStruct *page);

#endif
