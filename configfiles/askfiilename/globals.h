
#include <string.h>

//char	*fileName="data";

static void setAskEntry(GtkWidget* widget,gpointer ptr)
{
	printf(">>%i<<\n",(int)(long)ptr);
	printf("filname=%s\n",gtk_entry_get_text((GtkEntry*)filenamedialogText[ENTERFILENAMETXT]));

#ifdef _AskEntry_ISDIALOG_
	gtk_widget_destroy((GtkWidget*)filenamedialogWindow);
#else
	if((long)ptr<0)
		gtk_main_quit();
#endif
}







