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

#include "globals.h"

struct	menuDataStruct
{
	const char		*menuLabel;
	const char		*stockID;
	char			key;
	long			mod;
	void			*cb;
	const char		*widgetName;
	gpointer		userData;	
};

struct	contextStruct
{
	pageStruct	*page;
	GtkTreePath	*treepath;
	GtkTreeView *tree;
	unsigned	id;
};

enum	{CONTEXTNEWFILE=0,CONTEXTNEWFOLDER,CONTEXTBMNEW,CONTEXTOPEN,CONTEXTDELETE,CONTEXTDUP,CONTEXTEXTRACT,CONTEXTDISKMOUNT,CONTEXTDISKUNMOUNT,CONTEXTDISKEJECT,CONTEXTBMOPEN,CONTEXTBMDELETE,MAINFILENEW,MAINFILETERMINAL,MAINFILEPREFS,MAINFILEQUIT};

extern	menuDataStruct	menuData[];
extern	contextStruct	**contextMenus;

GtkWidget *createNewBox(int orient,bool homog,int spacing);
void buidMainGui(const char *startdir);
void populatePageStore(pageStruct *page);
void newIconView(pageStruct *page);
GtkWidget* newMenuItem(unsigned menunumber,GtkWidget *parent);
GtkWidget* newImageMenuItem(unsigned menunumber,GtkWidget *parent);
unsigned hashMimeType(char* cp);
GtkWidget *makeNewTab(char *name,pageStruct *page);
char *truncateWithElipses(char *str,unsigned int maxlen);
void updateTabLabel(pageStruct	*page);
void updateDiskList(void);
gboolean loadFiles(gpointer data);
void loadPixbufs(void);
void updateBMList(void);
bool checkCDROMChanged(void);

#endif
