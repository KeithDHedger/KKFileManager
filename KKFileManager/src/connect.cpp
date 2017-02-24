/*
 *
 * ©K. D. Hedger. Fri 24 Feb 14:15:11 GMT 2017 kdhedger68713@gmail.com

 * This file (connect.cpp) is part of KKFileManager.

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

#include "globals.h"

char		*userName=getenv("USER");
char		*password=NULL;
char		*server=NULL;
char		*shareName=NULL;
char		*portName=NULL;

GtkWidget	*connectRadio[4];
GtkWidget	*connectText[5];

GtkWidget	*connectWindow;

void makeConnectRadio(int widgnum,const char *label,bool onoff,GtkBox *box)
{
	if(widgnum==0)
		connectRadio[widgnum]=gtk_radio_button_new_with_label(NULL,label);
	else
		connectRadio[widgnum]=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)connectRadio[0],label);

	gtk_widget_set_name(connectRadio[widgnum],label);
	gtk_toggle_button_set_active((GtkToggleButton*)connectRadio[widgnum],onoff);
	gtk_box_pack_start(box,connectRadio[widgnum],false,false,0);
	g_signal_connect(G_OBJECT(connectRadio[widgnum]),"clicked",G_CALLBACK(setConnect),(void*)(long)(widgnum+3000));
}
 
void makeConnectText(int widgnum,const char *label,const char *defaulttxt,GtkBox *box,bool showlabel)
{
	GtkWidget	*hbox=createNewBox(NEWHBOX,false,0);
	GtkWidget	*pad=createNewBox(NEWHBOX,false,0);

	connectText[widgnum]=gtk_entry_new();
	if(defaulttxt!=NULL)
		gtk_entry_set_text((GtkEntry*)connectText[widgnum],defaulttxt);
	if((strlen(label)>0) && (showlabel==true))
		{
			gtk_box_pack_start((GtkBox*)hbox,gtk_label_new(label),false,false,0);
			gtk_box_pack_start((GtkBox*)hbox,pad,true,false,0);
			gtk_box_pack_start((GtkBox*)hbox,connectText[widgnum],false,false,0);
			gtk_box_pack_start(box,hbox,false,false,0);
		}
	else
		gtk_box_pack_start(box,connectText[widgnum],false,false,0);
}

void doConnect(GtkWidget* widget,gpointer data)
{
	GtkBox		*vbox;
	GtkWidget	*hbox;
	GtkBox		*tbox;
	GtkWidget	*item;

	GtkWidget *content;

	connectWindow=gtk_dialog_new();
	content=gtk_dialog_get_content_area((GtkDialog *)connectWindow);
	gtk_window_set_title((GtkWindow*)connectWindow,"New Connection");
	vbox=(GtkBox*)createNewBox(NEWVBOX,false,0);

	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("User\t\t"),false,false,0);
	makeConnectText(usertag,"",userName,tbox,false);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("Password\t"),false,false,0);
	makeConnectText(passtag,"",password,tbox,false);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("Server\t\t"),false,false,0);
	makeConnectText(servertag,"",server,tbox,false);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("Port\t\t\t"),false,false,0);
	makeConnectText(porttag,"",portName,tbox,false);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),gtk_label_new("Share\t\t"),false,false,0);
	makeConnectText(sharetag,"",shareName,tbox,false);
	gtk_box_pack_start(GTK_BOX(vbox),(GtkWidget*)tbox,true,true,4);
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,true,0),true,true,1);
	makeConnectRadio(smbtag,"SMB",true,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,true,0),true,true,1);
	makeConnectRadio(ftptag,"FTP",false,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,true,0),true,true,1);
	makeConnectRadio(sshtag,"SSH",false,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,true,0),true,true,1);
	makeConnectRadio(davtag,"DAV",false,tbox);
	gtk_box_pack_start(GTK_BOX(tbox),createNewBox(NEWHBOX,true,0),true,true,1);
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
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setConnect),(void*)-1);	

#ifdef _USEGTK3_
	item=gtk_button_new_with_mnemonic("_Cancel");
#else
	item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
#endif
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setConnect),(void*)-2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);
	gtk_box_pack_start(GTK_BOX(content),(GtkWidget*)vbox,true,true,2);
	gtk_widget_show_all((GtkWidget*)vbox);
	gtk_window_set_transient_for((GtkWindow*)connectWindow,(GtkWindow*)mainWindow);
	
}
