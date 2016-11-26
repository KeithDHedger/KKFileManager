
/******************************************************
*
*     ©keithhedger Thu 24 Nov 20:13:52 GMT 2016
*     kdhedger68713@gmail.com
*
*     prefs.cpp
* 
******************************************************/

#include "prefs.h"
#include "globals.h"

GtkWidget	*prefsText[3];

GtkWidget	*prefsWindow;

void makePrefsText(int widgnum,const char *label,char *defaulttxt,GtkBox *box,bool showlabel)
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

	prefsWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)prefsWindow,"Preferences");
	vbox=(GtkBox*)createNewBox(NEWVBOX,false,0);

	makePrefsText(TOOLBARLAYOUTTXT,"Toolbar Layout",toolBarLayout,vbox,true);
	makePrefsText(INCLUDEDISKLISTTXT,"Include Disk List",diskIncludePattern,vbox,true);
	makePrefsText(EXCLUDEDISKLISTTXT,"Exclude Disk List",diskExcludePattern,vbox,true);
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
	gtk_container_add(GTK_CONTAINER(prefsWindow),(GtkWidget*)vbox);
	gtk_widget_show_all(prefsWindow);
}
