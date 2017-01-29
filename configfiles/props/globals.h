#include <string.h>

//char *labelname="this is a staic label";

//char	*ownerName="keithhedger";
//char	*groupName="wheel";
//
//bool	oReadBit=true;
//bool	oWriteBit=true;
//bool	oExecuteBit=true;
//
//bool	gReadBit=true;
//bool	gWriteBit=false;
//bool	gExecuteBit=true;
//
//bool	rReadBit=true;
//bool	rWriteBit=false;
//bool	rExecuteBit=true;
//
//bool	setUIDBit=false;
//bool	setGIDBot=false;
//bool	stickyBit=false;
//
//bool	doRecursive=true;

//char	*filePath="/media/LinuxData/Development64/Projects/PrefsMaker/permissions/globals.h";
//char	*fileSize="123456";
//char	*fileModified="2017-01-28 15:07:51";
//char	*fileAccessed="2017-01-28 15:07:51";

void setFileProps(GtkWidget* widget,gpointer ptr)
{
	printf(">>%i<<\n",(int)(long)ptr);
#ifdef _FileProps_ISDIALOG_
	if((long)ptr<0)
		gtk_widget_destroy((GtkWidget*)filepropsWindow);
#endif
}

