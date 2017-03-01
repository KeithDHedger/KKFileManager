
/******************************************************
*
*     ©keithhedger Wed  1 Mar 17:27:33 GMT 2017
*     kdhedger68713@gmail.com
*
*     filenamedialog.cpp
* 
******************************************************/

#include "globals.h"

char		*fileName;
bool		validName;

GtkWidget	*filenamedialogText[1];

GtkWidget	*filenamedialogWindow;

void makeAskFilenameText(int widgnum,const char *label,const char *defaulttxt,GtkBox *box,bool showlabel)
{
	GtkWidget	*hbox=createNewBox(NEWHBOX,false,0);
	GtkWidget	*pad=createNewBox(NEWHBOX,false,0);

	filenamedialogText[widgnum]=gtk_entry_new();
	if(defaulttxt!=NULL)
		gtk_entry_set_text((GtkEntry*)filenamedialogText[widgnum],defaulttxt);
	if((strlen(label)>0) && (showlabel==true))
		{
			gtk_box_pack_start((GtkBox*)hbox,gtk_label_new(label),false,false,0);
			gtk_box_pack_start((GtkBox*)hbox,pad,true,false,0);
			gtk_box_pack_start((GtkBox*)hbox,filenamedialogText[widgnum],false,false,0);
			gtk_box_pack_start(box,hbox,false,false,0);
		}
	else
		gtk_box_pack_start(box,filenamedialogText[widgnum],false,false,0);
}

void doAskForFilename(GtkWidget* widget,gpointer data)
{
	GtkBox		*vbox;
	GtkWidget	*hbox;
	GtkBox		*tbox;
	GtkWidget	*item;

	GtkWidget *content;

	filenamedialogWindow=gtk_dialog_new();
	content=gtk_dialog_get_content_area((GtkDialog *)filenamedialogWindow);
	gtk_window_set_title((GtkWindow*)filenamedialogWindow,"Enter Filename");
	vbox=(GtkBox*)createNewBox(NEWVBOX,false,0);

	makeAskFilenameText(FILENAMETXT,"Enter Filename",fileName,vbox,true);
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,4);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,4);
#endif

	hbox=createNewBox(NEWHBOX,true,4);

#ifdef _USEGTK3_
	item=gtk_button_new_with_mnemonic("_Apply");
#else
	item=gtk_button_new_from_stock(GTK_STOCK_APPLY);
#endif

	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setAskFilename),(void*)-1);	

#ifdef _USEGTK3_
	item=gtk_button_new_with_mnemonic("_Cancel");
#else
	item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
#endif
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setAskFilename),(void*)-2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);
	gtk_box_pack_start(GTK_BOX(content),(GtkWidget*)vbox,true,true,2);
	gtk_widget_show_all((GtkWidget*)vbox);
	gtk_window_set_transient_for((GtkWindow*)filenamedialogWindow,(GtkWindow*)mainWindow);
	gtk_dialog_run((GtkDialog*)filenamedialogWindow);
}
