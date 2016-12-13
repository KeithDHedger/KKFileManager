/*
 *
 * ©K. D. Hedger. Mon  5 Dec 13:19:36 GMT 2016 kdhedger68713@gmail.com

 * This file (dialogs.cpp) is part of KKFileManager.

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

#include "dialogs.h"
#include "globals.h"

char		*fileName;
bool		validName;

GtkWidget	*askentryText[1];
GtkWidget	*askentryWindow;

void makeAskEntryText(int widgnum,const char *label,const char *defaulttxt,GtkBox *box,bool showlabel)
{
	GtkWidget	*hbox=createNewBox(NEWHBOX,false,0);
	GtkWidget	*pad=createNewBox(NEWHBOX,false,0);

	askentryText[widgnum]=gtk_entry_new();
	if(defaulttxt!=NULL)
		gtk_entry_set_text((GtkEntry*)askentryText[widgnum],defaulttxt);
	if((strlen(label)>0) && (showlabel==true))
		{
			gtk_box_pack_start((GtkBox*)hbox,gtk_label_new(label),false,false,0);
			gtk_box_pack_start((GtkBox*)hbox,pad,true,false,0);
			gtk_box_pack_start((GtkBox*)hbox,askentryText[widgnum],false,false,0);
			gtk_box_pack_start(box,hbox,false,false,0);
		}
	else
		gtk_box_pack_start(box,askentryText[widgnum],false,false,0);
}

void doAskForFilename(const char* filename)
{
	GtkBox		*vbox;
	GtkWidget	*hbox;
	GtkWidget	*item;

	GtkWidget *content;

	askentryWindow=gtk_dialog_new();
	content=gtk_dialog_get_content_area((GtkDialog *)askentryWindow);
	gtk_window_set_title((GtkWindow*)askentryWindow,"Enter Filename");
	vbox=(GtkBox*)createNewBox(NEWVBOX,false,0);

	makeAskEntryText(ENTERFILENAMETXT,"Enter Filename",filename,vbox,true);
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
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setAskEntry),(void*)-1);	

#ifdef _USEGTK3_
	item=gtk_button_new_with_mnemonic("_Cancel");
#else
	item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
#endif
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setAskEntry),(void*)-2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);
	gtk_box_pack_start(GTK_BOX(content),(GtkWidget*)vbox,true,true,2);
	gtk_widget_show_all((GtkWidget*)vbox);
	gtk_dialog_run((GtkDialog*)askentryWindow);
}
