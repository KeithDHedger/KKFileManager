/*
 *
 * ©K. D. Hedger. Wed 16 Nov 15:08:29 GMT 2016 kdhedger68713@gmail.com

 * This file (pages.h) is part of KKFileManager.

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

#ifndef _PAGES_
#define _PAGES_

#include <gtk/gtk.h>

struct pageStruct
{
	unsigned			pageID;
	GFile				*dirPath;
	GFileMonitor		*monitorDir;
	GtkWidget			*vBox;
	GtkScrolledWindow	*scrollBox;
	GtkIconView			*iconView;
	GtkListStore		*listStore;
	char				*thisFolder;
	GtkWidget			*tabBox;
	GtkLabel			*tabLabel;
	FILE				*fp;
	bool				doLoop;
	bool				toggleOff;
	bool				startedDrag;
	bool				stdBehaviour;
	guint				bupsignal;
	guint				bdownsignal;
	guint				selectsignal;
	GList				*fList;
	GList				*bList;
	int					fileCnt;
	int					fileType;
	int					uptoHere;
	int					fromHere;
	struct dirent		**fileList;
	GtkTreeIter			searchIter;
	char				searchString[NAME_MAX];
	bool				buttonDown;
	int					searchPath;
	pthread_t			thumbnailThread;
	bool				thumbnailThreadRunning;
};

unsigned getPageIdFromTabNum(int tabnum);
unsigned getPageIdFromTab(void);
pageStruct *getPageStructByIDFromList(unsigned pageid);
void addNewPage(char *startdir);
void monitorFolderForPage(pageStruct *page);
void flushFolderBuffer(pageStruct *page);
pageStruct *getPageFromCurrentTab(void);

#endif
