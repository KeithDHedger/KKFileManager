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

void setConnect(GtkWidget* widget,gpointer ptr)
{
	char	*command=NULL;

	printf(">>%i<<\n",(int)(long)ptr);
#ifdef _Connect_ISDIALOG_
//	for(int j=SMB1CHK;j<=DAV4CHK;j++)
//		{
//			//if((int)(long)ptr-1000!=j)
//				gtk_toggle_button_set_active((GtkToggleButton*)connectCheck[j],false);
//			//else
//		}
//	if((int)(long)ptr-1000!=j)
//	gtk_toggle_button_set_active((GtkToggleButton*)connectCheck[(int)(long)ptr-1000],true);
//	
//	asprintf(&command,"");	
	if((long)ptr<0)
		gtk_widget_destroy((GtkWidget*)connectWindow);
#endif
}

