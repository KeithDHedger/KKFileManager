/*
 *
 * ©K. D. Hedger. Tue 31 Jul 14:06:57 BST 2018 keithdhedger@gmail.com

 * This file (prefs.cpp) is part of KKFileManager.

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

GtkWidget	*prefsCheck[2];
GtkWidget	*prefsText[5];

GtkWidget	*prefsWindow;

void makePrefsCheck(int widgnum,const char *label,bool onoff,GtkBox *box)
{
	prefsCheck[widgnum]=gtk_check_button_new_with_label(label);
	gtk_widget_set_name(prefsCheck[widgnum],label);
	gtk_toggle_button_set_active((GtkToggleButton*)prefsCheck[widgnum],onoff);
	gtk_box_pack_start(box,prefsCheck[widgnum],false,false,0);
	g_signal_connect(G_OBJECT(prefsCheck[widgnum]),"clicked",G_CALLBACK(setPrefs),(void*)(long)(widgnum+1000));
}
 
void makePrefsText(int widgnum,const char *label,const char *defaulttxt,GtkBox *box,bool showlabel)
{
	GtkWidget	*hbox=createNewBox(NEWHBOX,false,0);
	GtkWidget	*pad=createNewBox(NEWHBOX,false,0);

	prefsText[widgnum]=gtk_entry_new();
	if(defaulttxt!=NULL)
		gtk_entry_set_text((GtkEntry*)prefsText[widgnum],defaulttxt);
	if((strlen(label)>0) && (showlabel==true))
		{
			gtk_box_pack_start((GtkBox*)hbox,gtk_label_new(label),false,false,0);
			gtk_box_pack_start((GtkBox*)hbox,pad,true,false,0);
			gtk_box_pack_start((GtkBox*)hbox,prefsText[widgnum],false,false,0);
			gtk_box_pack_start(box,hbox,false,false,0);
		}
	else
		gtk_box_pack_start(box,prefsText[widgnum],false,false,0);
}

void doPrefs(GtkWidget* widget,gpointer data)
{
	GtkBox		*vbox;
	GtkWidget	*hbox;
	GtkBox		*tbox;
	GtkWidget	*item;

	GtkWidget *content;

	prefsWindow=gtk_dialog_new();
	content=gtk_dialog_get_content_area((GtkDialog *)prefsWindow);
	gtk_window_set_title((GtkWindow*)prefsWindow,"Preferences");
	vbox=(GtkBox*)createNewBox(NEWVBOX,false,0);

	makePrefsText(TERMINALCOMMANDTXT,"Terminal Command",terminalCommand,vbox,true);
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,4);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,4);
#endif
	makePrefsText(TOOLBARLAYOUTTXT,"Toolbar Layout",toolBarLayout,vbox,true);
	makePrefsText(INCLUDEDISKLISTTXT,"Include Disk List",diskIncludePattern,vbox,true);
	makePrefsText(EXCLUDEDISKLISTTXT,"Exclude Disk List",diskExcludePattern,vbox,true);
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,4);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,4);
#endif
	makePrefsCheck(EXECUTEONCLICK,"Execute on click",executeOnClick,vbox);
#ifdef _USEGTK3_
	gtk_box_pack_start(GTK_BOX(vbox),gtk_separator_new(GTK_ORIENTATION_HORIZONTAL),true,true,4);
#else
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,4);
#endif
	tbox=(GtkBox*)createNewBox(NEWHBOX,false,0);
	makePrefsCheck(NONETPICS,"No Thumbnails On",noThumbs,tbox);
	makePrefsText(EXCLUDENETPICSON,"",noNetPicsOn,tbox,true);
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
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)-1);	

#ifdef _USEGTK3_
	item=gtk_button_new_with_mnemonic("_Cancel");
#else
	item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
#endif
	gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
	g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setPrefs),(void*)-2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);
	gtk_box_pack_start(GTK_BOX(content),(GtkWidget*)vbox,true,true,2);
	gtk_widget_show_all((GtkWidget*)vbox);
	gtk_window_set_transient_for((GtkWindow*)prefsWindow,(GtkWindow*)mainWindow);
	gtk_dialog_run((GtkDialog*)prefsWindow);
}
