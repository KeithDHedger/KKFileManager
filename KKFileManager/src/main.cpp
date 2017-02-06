/*
 *
 * ©K. D. Hedger. Mon  5 Dec 13:02:43 GMT 2016 kdhedger68713@gmail.com

 * This file (main.cpp) is part of KKFileManager.

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

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <gtk/gtk.h>
#include <map>

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
//#include <sys/msg.h>

#include "globals.h"
#include "gui.h"

bool	singleOverRide=false;
gchar	*mounts=NULL;

void themeChanged(GtkIconTheme *icon_theme,gpointer user_data)
{
	GtkWidget	*widg;
	unsigned	pageid;
	pageStruct	*page;

	pixBuffCache.clear();
	loadPixbufs();
	updateBMList();
	updateDiskList();
	for(int j=0;j<gtk_notebook_get_n_pages(mainNotebook);j++)
		{
			widg=gtk_notebook_get_nth_page(mainNotebook,j);
			pageid=(unsigned)(long)g_object_get_data((GObject*)widg,"pageid");
			page=getPageStructByIDFromList(pageid);
			populatePageStore(page);
		}
}

void loadPrefs(void)
{
	char	*filename;

	sinkReturn=asprintf(&filename,"%s/%s",getenv("HOME"),APPFOLDENAME);
	g_mkdir_with_parents(filename,493);
	free(filename);
	sinkReturn=asprintf(&filename,"%s/%s/kkfiemanager.rc",getenv("HOME"),APPFOLDENAME);

	loadVarsFromFile(filename,kkfilemanager_rc);
	if(windowAllocData!=NULL)
		sscanf(windowAllocData,"%i %i %i %i",(int*)&windowWidth,(int*)&windowHeight,(int*)&windowX,(int*)&windowY);

	free(filename);
}

int getWorkspaceNumber(void)
{
	GdkDisplay		*display;
	GdkWindow		*root_win;
	Atom			_net_current_desktop,type;
	int				format;
	unsigned long	n_items, bytes_after;
	unsigned char	*data_return=0;
	int				retnum=0;

	display=gdk_screen_get_display(gdk_screen_get_default());
	root_win=gdk_screen_get_root_window(gdk_screen_get_default());

	_net_current_desktop=gdk_x11_get_xatom_by_name_for_display(display,"_NET_CURRENT_DESKTOP");

	XGetWindowProperty(GDK_DISPLAY_XDISPLAY(display),GDK_WINDOW_XID(root_win),_net_current_desktop,0,G_MAXLONG,False,XA_CARDINAL,&type,&format,&n_items,&bytes_after,&data_return);

	if(type==XA_CARDINAL && format==32 && n_items>0)
		{
			retnum=(int)data_return[0];
			XFree(data_return);
		}
	return retnum;
}

void activate(GApplication *application)
{
	if(mainWindow!=NULL)
		gtk_window_present((GtkWindow*)mainWindow);
	//getMsg();
}

void open(GApplication *application,GFile** files,gint n_files,const gchar *hint)
{
	char	*filepath=NULL;

	g_application_hold(application);

	if(mainWindow!=NULL)
		gtk_window_present((GtkWindow*)mainWindow);

	for(int i=0; i<n_files; i++)
		{
			filepath=g_file_get_path(files[i]);
			if(filepath!=NULL)
				addNewPage(filepath);
		}

//	if(waitForFinish==true)
//		{
//			waitForFinish=false;
//			sendOK();
//		}
	g_application_release(application);
}

gboolean checkDisks(gpointer data)
{
	gchar	*temp=NULL;
	g_file_get_contents("/proc/mounts",&temp,NULL,NULL);
	if(strcmp(temp,mounts)!=0)
		{
			if(mounts!=NULL)
				g_free(mounts);
			mounts=temp;
			updateDiskList();
		}
	else
		{
			g_free(temp);
			if(checkCDROMChanged()==true)
				updateDiskList();
			else if(checkDisksChanged()==true)
				updateDiskList();
		}

	return(true);
}

void appStart(GApplication  *application,gpointer data)
{
	GtkIconInfo	*info=NULL;

	g_application_hold(application);

	toolBarLayout=strdup("NUBFHLR");
	diskIncludePattern=strdup("*sd?*");
	diskExcludePattern=strdup("*");
	terminalCommand=strdup("xterm -title ");

	defaultTheme=gtk_icon_theme_get_default();
	gnomeTheme=gtk_icon_theme_new();
	gtk_icon_theme_set_custom_theme(gnomeTheme,"gnome");

	info=gtk_icon_theme_lookup_icon(gnomeTheme,"emblem-symbolic-link",16,(GtkIconLookupFlags)0);
	symLink=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,16,NULL);
#ifdef _USEGTK3_
	g_object_unref(info);
#else
	gtk_icon_info_free(info);
#endif
	info=gtk_icon_theme_lookup_icon(gnomeTheme,"emblem-unreadable",16,(GtkIconLookupFlags)0);
	brokenLink=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,16,NULL);
#ifdef _USEGTK3_
	g_object_unref(info);
#else
	gtk_icon_info_free(info);
#endif

	magicInstance=magic_open(MAGIC_MIME_TYPE);
	magic_load(magicInstance,NULL);



mime_type_init();



	iconSize=ICONSIZE;
	loadPrefs();
#ifdef _USEGTK3_
	iconSize3=1;
#else
	iconSize3=2;
#endif

	loadPixbufs();

	buidMainGui(NULL);

	mainClipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	testpb=gdk_pixbuf_new_from_file_at_size("/usr/share/icons/gnome/48x48/mimetypes/application-x-executable.png",-1,48,NULL);
	g_signal_connect_after(G_OBJECT(defaultTheme),"changed",G_CALLBACK(themeChanged),NULL);

	mounts=strdup("");
	g_timeout_add(1000,checkDisks,NULL);
}

int main(int argc,char **argv)
{
	char			*origpath=NULL;
	int				status;
	char			*dbusname;
	GOptionContext	*context;



	GOptionEntry	entries[]=
{
    {"multiple",'m',0,G_OPTION_ARG_NONE,&singleOverRide,"Multiple instance mode",NULL},
    {"sessionid",'i',0,G_OPTION_ARG_INT,&sessionID,"Set an ID to be used for (new) instance",NULL},
    { NULL }
};

	context=g_option_context_new(NULL);
	g_option_context_add_main_entries(context,entries,NULL);
	g_option_context_set_help_enabled(context,true); 
	g_option_context_parse(context,&argc,&argv,NULL);

	gtk_init(&argc,&argv);

	if(sessionID==-1)
		sinkReturn=asprintf(&dbusname,"org.keithhedger%i." APPEXECNAME,getWorkspaceNumber());
	else
		sinkReturn=asprintf(&dbusname,"org.keithhedger%i." APPEXECNAME,sessionID);

	if(singleOverRide==true)
		mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_NON_UNIQUE|G_APPLICATION_HANDLES_OPEN));
	else
		mainApp=g_application_new(dbusname,(GApplicationFlags)(G_APPLICATION_HANDLES_OPEN));

	if(argc==1)
		openDefault=true;
	else
		openDefault=false;

	g_signal_connect(mainApp,"activate",G_CALLBACK(activate),NULL);
	g_signal_connect(mainApp,"startup",G_CALLBACK(appStart),NULL);
	g_signal_connect(mainApp,"open",G_CALLBACK(open),NULL);

	status=g_application_run(mainApp,argc,argv);
	g_object_unref(mainApp);
	free(origpath);
	if(G_IS_OBJECT(defaultTheme))
		g_object_unref(defaultTheme);
	magic_close(magicInstance);
	if(mounts!=NULL)
		g_free(mounts);
	return(status);
}