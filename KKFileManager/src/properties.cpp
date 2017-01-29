
/******************************************************
*
*     ©keithhedger Sun 29 Jan 15:11:24 GMT 2017
*     kdhedger68713@gmail.com
*
*     properties.cpp
* 
******************************************************/

#include "properties.h"
#include "globals.h"

GtkWidget	*filepropsCheck[13];
GtkWidget	*filepropsText[2];

GtkWidget	*filepropsWindow;

GtkWidget *createNewBox(int orient,bool homog,int spacing)
{
	GtkWidget	*retwidg=NULL;

#ifdef _USEGTK3_
	if(orient==NEWVBOX)
		retwidg=gtk_box_new(GTK_ORIENTATION_VERTICAL,spacing);
	else
		retwidg=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,spacing);
	gtk_box_set_homogeneous((GtkBox*)retwidg,homog);
#else
	if(orient==NEWVBOX)
		retwidg=gtk_vbox_new(homog,spacing);
	else
		retwidg=gtk_hbox_new(homog,spacing);
#endif
	return(retwidg);
}

void makeFilePropsCheck(int widgnum,const char *label,bool onoff,GtkBox *box)
{
	filepropsCheck[widgnum]=gtk_check_button_new_with_label(label);
	gtk_widget_set_name(filepropsCheck[widgnum],label);
	gtk_toggle_button_set_active((GtkToggleButton*)filepropsCheck[widgnum],onoff);
	gtk_box_pack_start(box,filepropsCheck[widgnum],false,false,0);
	g_signal_connect(G_OBJECT(filepropsCheck[widgnum]),"clicked",G_CALLBACK(setFileProps),(void*)(long)(widgnum+1000));
}
 
void makeFilePropsText(int widgnum,const char *label,char *defaulttxt,GtkBox *box,bool showlabel)
{
	GtkWidget	*hbox=createNewBox(NEWHBOX,false,0);
	GtkWidget	*pad=createNewBox(NEWHBOX,false,0);

	filepropsText[widgnum]=gtk_entry_new();
	if(defaulttxt!=NULL)
		gtk_entry_set_text((GtkEntry*)filepropsText[widgnum],defaulttxt);
	if((strlen(label)>0) && (showlabel==true))
		{
			gtk_box_pack_start((GtkBox*)hbox,gtk_label_new(label),false,false,0);
			gtk_box_pack_start((GtkBox*)hbox,pad,true,false,0);
			gtk_box_pack_start((GtkBox*)hbox,filepropsText[widgnum],false,false,0);
			gtk_box_pack_start(box,hbox,false,false,0);
		}
	else
		gtk_box_pack_start(box,filepropsText[widgnum],false,false,0);
}

void doFileProps(GtkWidget* widget,gpointer data)
{
	GtkBox		*vbox;
	GtkWidget	*hbox;
	GtkBox		*tbox;
	GtkWidget	*item;

	filepropsWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)filepropsWindow,"File Properties");
	vbox=(GtkBox*)createNewBox(NEWVBOX,false,0);

#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,4);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,4);
#endif
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,10);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new(filePath),false,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,10);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,4);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,4);
#endif
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("Owner\t"),false,false,0);
	makeFilePropsText(TXT0,"",ownername,tbox,false);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("Group\t"),false,false,0);
	makeFilePropsText(TXT1,"",groupname,tbox,false);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,4);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,4);
#endif
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("Owner\t"),false,false,0);
	makeFilePropsCheck(READ1CHK,"Read",oReadBit,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,25);
	makeFilePropsCheck(WRITE2CHK,"Write",oWriteBit,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,25);
	makeFilePropsCheck(EXECUTE3CHK,"Execute",oExecuteBit,tbox);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("Group\t"),false,false,0);
	makeFilePropsCheck(READ4CHK,"Read",gReadBit,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,25);
	makeFilePropsCheck(WRITE5CHK,"Write",gWriteBit,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,25);
	makeFilePropsCheck(EXECUTE6CHK,"Execute",gExecuteBit,tbox);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("Other\t"),false,false,0);
	makeFilePropsCheck(READ7CHK,"Read",rReadBit,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,25);
	makeFilePropsCheck(WRITE8CHK,"Write",rWriteBit,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,25);
	makeFilePropsCheck(EXECUTE9CHK,"Execute",rExecuteBit,tbox);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,4);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,4);
#endif
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("World\t"),false,false,0);
	makeFilePropsCheck(SUID10CHK,"SUID",setUIDBit,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,25);
	makeFilePropsCheck(SGID11CHK,"SGID",setGIDBot,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,25);
	makeFilePropsCheck(STICKY12CHK,"Sticky",stickyBit,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,10);
	makeFilePropsCheck(RECURSIVE13CHK,"Recursive",doRecursive,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,false,0),false,false,10);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,4);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,4);
#endif
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("File Size\t\t"),false,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new(fileSize),false,false,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("Modified\t\t"),false,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new(fileModified),false,false,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("Accessed\t"),false,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new(fileAccessed),false,false,0);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
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
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setFileProps),(void*)-1);	

#ifdef _USEGTK3_
	item=gtk_button_new_with_mnemonic("_Cancel");
#else
	item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
#endif
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setFileProps),(void*)-2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);
	gtk_container_add(GTK_CONTAINER(filepropsWindow),(GtkWidget*)vbox);
	gtk_widget_show_all(filepropsWindow);
}
