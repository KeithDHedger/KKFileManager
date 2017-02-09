/******************************************************
*
*     ©keithhedger Tue  7 Feb 17:16:04 GMT 2017
*     kdhedger68713@gmail.com
*
*     properties.h
* 
******************************************************/

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
extern bool multipleFiles;
extern const char *fileMime;
extern char *filePath;
extern char *fileSize;
extern char *fileModified;
extern char *fileAccessed;

extern GtkWidget	*filepropsCheck[];
extern GtkWidget	*filepropsText[];

extern GtkWidget	*filepropsWindow;

void doFileProps(GtkWidget* widget,gpointer data);

#endif

