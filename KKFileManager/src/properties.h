/*
 *
 * ©K. D. Hedger. Sun 29 Jan 18:23:37 GMT 2017 kdhedger68713@gmail.com

 * This file (properties.h) is part of KKFileManager.

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

#ifndef _PROPERTIES_
#define _PROPERTIES_

enum {READ1CHK=0,WRITE2CHK,EXECUTE3CHK,READ4CHK,WRITE5CHK,EXECUTE6CHK,READ7CHK,WRITE8CHK,EXECUTE9CHK,SUID10CHK,SGID11CHK,STICKY12CHK,RECURSIVE13CHK};
enum {READ1CHKCBID=1000,WRITE2CHKCBID,EXECUTE3CHKCBID,READ4CHKCBID,WRITE5CHKCBID,EXECUTE6CHKCBID,READ7CHKCBID,WRITE8CHKCBID,EXECUTE9CHKCBID,SUID10CHKCBID,SGID11CHKCBID,STICKY12CHKCBID,RECURSIVE13CHKCBID};
enum {TXT0=0,TXT1};

#define _FileProps_ISDIALOG_
extern char *ownerName;
extern char *groupName;
extern bool oReadBit;
extern bool oWriteBit;
extern bool oExecuteBit;
extern bool gReadBit;
extern bool gWriteBit;
extern bool gExecuteBit;
extern bool rReadBit;
extern bool rWriteBit;
extern bool rExecuteBit;
extern bool setUIDBit;
extern bool setGIDBit;
extern bool stickyBit;
extern bool doRecursive;
extern char *filePath;
extern char *fileSize;
extern char *fileModified;
extern char *fileAccessed;

extern GtkWidget	*filepropsCheck[];
extern GtkWidget	*filepropsText[];

extern GtkWidget	*filepropsWindow;

void doFileProps(GtkWidget* widget,gpointer data);

#endif

