/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:16:28 GMT 2016 kdhedger68713@gmail.com

 * This file (gui.cpp) is part of KKFileManager.

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
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include "globals.h"

GdkPixbuf *pixbuft;

menuDataStruct	menuData[]=
	{
//context
		{"New File",GTK_STOCK_NEW,0,0,NULL,"newfilemenu",NULL},
		{"New Folder",GTK_STOCK_DIRECTORY,0,0,NULL,"newfoldermenu",NULL},
		{"New Bookmark",GTK_STOCK_NEW,0,0,NULL,"newbmmenu",NULL},
		{"Open",GTK_STOCK_OPEN,0,0,NULL,"openmenu",NULL},
		{"Delete",GTK_STOCK_DELETE,0,0,NULL,"deletemenu",NULL},
		{"Duplicate",GTK_STOCK_COPY,0,0,NULL,"duplicatemenu",NULL},
		{"Extract",GTK_STOCK_CONVERT,0,0,NULL,"extractmenu",NULL},
//disk context
		{"Mount",GTK_STOCK_HARDDISK,0,0,NULL,"mountdiskmenu",NULL},
		{"Un-Mount",GTK_STOCK_HARDDISK,0,0,NULL,"unmountdiskmenu",NULL},
		{"Eject",GTK_STOCK_HARDDISK,0,0,NULL,"ejectdiskmenu",NULL},
//bm context
		{"Open",GTK_STOCK_OPEN,0,0,NULL,"openbmmenu",NULL},
		{"Delete",GTK_STOCK_DELETE,0,0,NULL,"deletebmmenu",NULL},

//main
		{"New Tab",GTK_STOCK_NEW,0,0,(void*)&goNew,"newtabmenu",NULL},
		{"Terminal",GTK_STOCK_EXECUTE,0,0,(void*)&runTerminalHere,"terminalmenu",NULL},
		{"Preferences",GTK_STOCK_PREFERENCES,0,0,(void*)&doPrefs,"prefsmenu",NULL},
		{"Quit",GTK_STOCK_QUIT,0,0,(void*)&doShutdown,"quitmenu",NULL}
	};



contextStruct	**contextMenus;
GtkTargetEntry	*target=NULL;

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

char	keyStrBuffer[32];
void setHotKeyString(unsigned menunumber)
{
	const char	*ckeystr="";
	const char	*skeystr="";

	if(menuData[menunumber].key!=0)
		{
			if(GDK_CONTROL_MASK & menuData[menunumber].mod)
				ckeystr="Ctrl+";
			if(GDK_SHIFT_MASK & menuData[menunumber].mod)
				skeystr="Shift+";

			sprintf(keyStrBuffer,"%s%s%s",skeystr,ckeystr,gdk_keyval_name(menuData[menunumber].key));
		}
	else
		keyStrBuffer[0]=0;
}

GtkWidget* newMenuItem(unsigned menunumber,GtkWidget *parent)
{
	GtkWidget	*menu;
#ifdef _USEGTK3_
	GtkWidget	*menuhbox;
	GtkWidget	*pad;
	GtkWidget	*image;
	GtkWidget	*ritelabel;
	char		*labelwithspace;

	menu=gtk_menu_item_new_with_mnemonic(menuData[menunumber].menuLabel);
	if((showMenuIcons==true) && (menuData[menunumber].stockID!=NULL))
		{
			setHotKeyString(menunumber);
			gtk_widget_destroy(gtk_bin_get_child(GTK_BIN(menu)));
			menuhbox=createNewBox(NEWHBOX,false,0);
			pad=createNewBox(NEWHBOX,false,0);

			image=gtk_image_new_from_icon_name(menuData[menunumber].stockID,GTK_ICON_SIZE_MENU);
			gtk_box_pack_start((GtkBox*)menuhbox,image,false,false,0);

			asprintf(&labelwithspace," %s",menuData[menunumber].menuLabel);
			gtk_box_pack_start(GTK_BOX(menuhbox),gtk_label_new_with_mnemonic(labelwithspace),false,false,0);
			free(labelwithspace);
			gtk_box_pack_start(GTK_BOX(menuhbox),pad,true,true,0);

			ritelabel=gtk_label_new(keyStrBuffer);
			gtk_widget_set_sensitive(ritelabel,false);
			gtk_box_pack_start(GTK_BOX(menuhbox),ritelabel,false,false,8);

			gtk_container_add(GTK_CONTAINER(menu),menuhbox);
		}
#else
	if(menuData[menunumber].stockID!=NULL)
		menu=gtk_image_menu_item_new_from_stock(menuData[menunumber].stockID,NULL);
	else
		menu=gtk_menu_item_new_with_mnemonic(menuData[menunumber].menuLabel);
#endif
	if(menuData[menunumber].key>0)
		gtk_widget_add_accelerator((GtkWidget *)menu,"activate",accgroup,menuData[menunumber].key,(GdkModifierType)menuData[menunumber].mod,GTK_ACCEL_VISIBLE);

	if(parent!=NULL)
		gtk_menu_shell_append(GTK_MENU_SHELL(parent),menu);
	if(menuData[menunumber].cb!=NULL)
		g_signal_connect(G_OBJECT(menu),"activate",G_CALLBACK(menuData[menunumber].cb),menuData[menunumber].userData);
	gtk_widget_set_name(menu,menuData[menunumber].widgetName);

	return(menu);
}

GtkWidget* newImageMenuItem(unsigned menunumber,GtkWidget *parent)
{
	GtkWidget	*menu;
#ifdef _USEGTK3_
	GtkWidget	*menuhbox;
	GtkWidget	*pad;
	GtkWidget	*image;
	GtkWidget	*ritelabel;
	char		*labelwithspace;

	menu=gtk_menu_item_new_with_mnemonic(menuData[menunumber].menuLabel);
	if(showMenuIcons==true)
		{
			setHotKeyString(menunumber);
			gtk_widget_destroy(gtk_bin_get_child(GTK_BIN(menu)));
			menuhbox=createNewBox(NEWHBOX,false,0);
			pad=createNewBox(NEWHBOX,false,0);

			image=gtk_image_new_from_icon_name(menuData[menunumber].stockID,GTK_ICON_SIZE_MENU);
			gtk_box_pack_start((GtkBox*)menuhbox,image,false,false,0);

			asprintf(&labelwithspace," %s",menuData[menunumber].menuLabel);
			gtk_box_pack_start(GTK_BOX(menuhbox),gtk_label_new_with_mnemonic(labelwithspace),false,false,0);
			free(labelwithspace);
			gtk_box_pack_start(GTK_BOX(menuhbox),pad,true,true,0);

			ritelabel=gtk_label_new(keyStrBuffer);
			gtk_widget_set_sensitive(ritelabel,false);
			gtk_box_pack_start(GTK_BOX(menuhbox),ritelabel,false,false,8);

			gtk_container_add(GTK_CONTAINER(menu),menuhbox);
		}
#else
	GtkWidget	*image;
	menu=gtk_image_menu_item_new_with_mnemonic(menuData[menunumber].menuLabel);
	image=gtk_image_new_from_stock(menuData[menunumber].stockID,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menu,image);
#endif
	if(menuData[menunumber].key>0)
		gtk_widget_add_accelerator((GtkWidget *)menu,"activate",accgroup,menuData[menunumber].key,(GdkModifierType)menuData[menunumber].mod,GTK_ACCEL_VISIBLE);

	if(parent!=NULL)
		gtk_menu_shell_append(GTK_MENU_SHELL(parent),menu);
	if(menuData[menunumber].cb!=NULL)
		g_signal_connect(G_OBJECT(menu),"activate",G_CALLBACK(menuData[menunumber].cb),menuData[menunumber].userData);
	if(menuData[menunumber].widgetName!=NULL)
		gtk_widget_set_name(menu,menuData[menunumber].widgetName);

	return(menu);
}

void switchPage(GtkNotebook *notebook,gpointer arg1,guint arg2,gpointer user_data)
{

	GtkWidget	*widg;
	widg=gtk_notebook_get_nth_page(notebook,arg2);

	pageStruct	*page=getPageStructByIDFromList((unsigned)(long)g_object_get_data((GObject*)widg,"pageid"));
	if(page!=NULL)
		{
			gtk_entry_set_text(locationTextBox,page->thisFolder);
			gtk_editable_set_position((GtkEditable*)locationTextBox,-1);
		}
}

void setNewPagePixbuf(GdkPixbuf *pixbuf,const char *type,const char *path,bool isdir,pageStruct *page)
{
	GtkTreeIter iter;

	gtk_list_store_append(page->listStore, &iter);
	if(pixbuf==NULL)
		pixbuf=genericText;
	gtk_list_store_set(page->listStore,&iter,PIXBUF_COLUMN,pixbuf,TEXT_COLUMN,type,FILEPATH,path,ISDIR,isdir,-1);
}

char *getMimeType(const char *path)
{
	char	*retdata;
	gboolean	uncertain=false;

	retdata=g_content_type_guess(path,NULL,0,&uncertain);
//printf("g_content_type_guess=%s\n",retdata);
	if(uncertain==false)
		return(retdata);
	
	retdata=strdup(magic_file(magicInstance,path));
//printf("magic_file=%s\n",retdata);
	return(retdata);
}

unsigned hashMimeType(char* cp)
{
	unsigned hash=5381;
	while(*cp)
		hash=33*hash^*cp++;
	return(hash);
}

GdkPixbuf* getPixBuf(const char *file)
{
	GIcon			*icon=NULL;
	GtkIconInfo		*info=NULL;
	GdkPixbuf		*pb=NULL;
	char			*mime;
	unsigned		hash;
	bool			issymlink=false;
	char			*symlink=NULL;
	char			*newf=NULL;
	bool			isbrokenlink=false;

//return(testpb);
	mime=getMimeType(file);
//mime=strdup("application-octet-stream");
//printf("mime=%s file %s\n",mime,file);
	if(g_file_test(file,G_FILE_TEST_IS_SYMLINK)==true)
		{
			issymlink=true;
			free(mime);
			newf=realpath(file,NULL);
			if(newf==NULL)
				{
					mime=strdup("application-octet-stream");
					hash=hashMimeType(mime);
					isbrokenlink=true;
				}
			else
				{
					mime=getMimeType(newf);
					symlink=strdup(mime);
					symlink=strncpy(symlink,"sym",3);
					hash=hashMimeType(symlink);
					free(symlink);
					free(newf);
				}
		}
	else
		hash=hashMimeType(mime);

	if(pixBuffCache.find(hash)!=pixBuffCache.end())
		{
			free(mime);
			return(pixBuffCache.find(hash)->second);
		}

	icon=g_content_type_get_icon(mime);
	info=gtk_icon_theme_lookup_by_gicon(defaultTheme,icon,48,(GtkIconLookupFlags)0);
	if(info==NULL)
		pb=genericText;
	else
		pb=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,48,NULL);

	if(issymlink==true)
		gdk_pixbuf_composite(symLink,pb,48-16,48-16,16,16,48-16,48-16,1.0,1.0,GDK_INTERP_NEAREST,255);

	if(isbrokenlink==true)
		gdk_pixbuf_composite(brokenLink,pb,0,48-16,16,16,0,48-16,1.0,1.0,GDK_INTERP_NEAREST,255);

	pixBuffCache[hash]=pb;
	free(mime);
	g_object_unref(icon);
	if(info!=NULL)
#ifndef _USEGTK3_
		gtk_icon_info_free(info);
#else
		g_object_unref(info);
#endif
	return(pb);
}

gboolean loadFiles(gpointer data)
{
	char		buffer[2048];
	GdkPixbuf	*pixbuf;
	pageStruct	*page=(pageStruct*)data;

	if(page->fp==NULL)
		{
			page->doLoop=false;
			return(false);
		}

	if(page->doLoop==false)
		{
			pclose(page->fp);
			page->fp=NULL;
			return(false);
		}

	if(page==NULL)
		{
			pclose(page->fp);
			page->fp=NULL;
			page->doLoop=false;
			return(false);
		}

	for(int j=0;j<LOADICONCNT;j++)
		{
			if(fgets(buffer,2048,page->fp))
				{
					if(strlen(buffer)>0)
						buffer[strlen(buffer)-1]=0;
					pixbuf=getPixBuf(buffer);
					setNewPagePixbuf(pixbuf,basename(buffer),buffer,false,page);
				}
			else
				{
					page->doLoop=false;
					return(true);
				}
		}
	return(true);
}

void populatePageStore(pageStruct *page)
{
	GdkPixbuf	*pixbuf;
	FILE		*fp=NULL;
	char		*command;
	char		buffer[2048];
	int			cnt=0;

	flushFolderBuffer(page);

	gtk_list_store_clear(page->listStore);
	asprintf(&command,"find \"%s\" -maxdepth 1 -mindepth 1 -type d -follow -not -path '*/\\.*'|sort",page->thisFolder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						buffer[strlen(buffer)-1]=0;
					pixbuf=getPixBuf(buffer);
					setNewPagePixbuf(pixbuf,basename(buffer),buffer,true,page);
					cnt++;
				}
			pclose(fp);
		}
	free(command);
	asprintf(&command,"find \"%s\" -maxdepth 1 -mindepth 1 -not -type d -follow -not -path '*/\\.*'|sort",page->thisFolder);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						buffer[strlen(buffer)-1]=0;
					pixbuf=getPixBuf(buffer);
					setNewPagePixbuf(pixbuf,basename(buffer),buffer,false,page);
					cnt++;
					if(cnt>LOADICONCNT)
					{
						page->fp=fp;
						page->doLoop=true;
						g_timeout_add(50,loadFiles,(gpointer)page);
						break;
					}
				}
		}
	free(command);
	gtk_widget_show_all((GtkWidget*)page->scrollBox);
}

void newIconView(pageStruct *page)
{
	char	buffer[64];
	sprintf(buffer,"GtkIconView-%u",pageCnt);

	page->listStore=gtk_list_store_new(NUMCOLS,G_TYPE_STRING,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_BOOLEAN);
	page->iconView=(GtkIconView*)gtk_icon_view_new();
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(page->iconView),PIXBUF_COLUMN);
	gtk_icon_view_set_text_column(GTK_ICON_VIEW(page->iconView),TEXT_COLUMN);
	gtk_icon_view_set_model(GTK_ICON_VIEW(page->iconView),GTK_TREE_MODEL(page->listStore));
	gtk_icon_view_set_item_padding(GTK_ICON_VIEW(page->iconView),0);
	g_signal_connect(page->iconView,"item-activated",G_CALLBACK(selectItem),page);	
	g_signal_connect(page->iconView,"button-press-event",G_CALLBACK(buttonDown),page);	
	populatePageStore(page);

	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(page->iconView),GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_item_orientation(GTK_ICON_VIEW(page->iconView),GTK_ORIENTATION_VERTICAL);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(page->iconView),-1);

	target=(GtkTargetEntry*)calloc(1,sizeof(GtkTargetEntry));
	target->target=(gchar*)"InternalTarget";
	target->flags=GTK_TARGET_SAME_APP;
	target->info=0;
	gtk_icon_view_enable_model_drag_source(page->iconView,GDK_BUTTON1_MASK,target,1,(GdkDragAction)(GDK_ACTION_COPY|GDK_ACTION_MOVE|GDK_ACTION_LINK));
	gtk_icon_view_enable_model_drag_dest(page->iconView,target,1,(GdkDragAction)(GDK_ACTION_COPY|GDK_ACTION_MOVE|GDK_ACTION_LINK));

	g_signal_connect(G_OBJECT(page->iconView),"drag-drop",G_CALLBACK(doDrop),(gpointer)page);

	gtk_icon_view_set_item_width(page->iconView,iconSize);
	gtk_icon_view_set_column_spacing(page->iconView,iconPadding);
	gtk_widget_set_name((GtkWidget*)page->iconView,buffer);
}

void setUpContextMenus(void)
{
	unsigned	menucnt;	
	menucnt=MAINFILENEW;//TODO//add user items
	contextMenus=(contextStruct**)calloc(menucnt,sizeof(contextStruct*));
	for(unsigned j=0;j<menucnt;j++)//TODO//manky code
		{
			contextMenus[j]=(contextStruct*)calloc(1,sizeof(contextStruct));
			contextMenus[j]->id=j;
		}
}

//truncate tabname with elipses
char *truncateWithElipses(char *str,unsigned int maxlen)
{
	char	*retstr=NULL;
	char	*front,*back;
	int		sides;

	if(g_utf8_validate(str,-1,NULL)==true)
		{
			if(g_utf8_strlen(str,-1)>maxlen)
				{
					sides=(maxlen-5)/2;
					front=g_utf8_substring(str,0,sides);
					back=g_utf8_substring(str,g_utf8_strlen(str,-1)-sides,g_utf8_strlen(str,-1));
					sinkReturn=asprintf(&retstr,"%s ... %s",front,back);
					free(front);
					free(back);
				}
			else
				retstr=strdup(str);
		}
	else
		retstr=strdup(str);

	return(retstr);
}

GtkWidget *makeNewTab(char *name,pageStruct *page)
{
	GtkWidget	*hbox;
	char		*correctedname;
	GtkWidget	*evbox=gtk_event_box_new();
	GtkWidget	*close=gtk_image_new_from_icon_name(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);

	GtkWidget	*button=gtk_button_new();

	hbox=createNewBox(NEWHBOX,false,0);
	correctedname=truncateWithElipses(name,maxTabChars);

	page->tabLabel=(GtkLabel*)gtk_label_new(correctedname);
	free(correctedname);

	gtk_button_set_relief((GtkButton*)button,GTK_RELIEF_NONE);
	gtk_box_pack_start(GTK_BOX(hbox),(GtkWidget*)page->tabLabel,false,false,0);

	gtk_button_set_focus_on_click(GTK_BUTTON(button),FALSE);
	gtk_container_add(GTK_CONTAINER(button),close);

	gtk_box_pack_start(GTK_BOX(hbox),button,false,false,0);
	gtk_container_add(GTK_CONTAINER(evbox),hbox);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(closeTab),(void*)page);
	//g_signal_connect(G_OBJECT(evbox),"button-press-event",G_CALLBACK(tabPopUp),(void*)page);//context menu?

	gtk_widget_show_all(evbox);

	return(evbox);
}

void updateBMList(void)
{
	GtkTreeIter	iter;
	FILE		*fp;
	char		*filepath;
	char		buffer[PATH_MAX+1];

	gtk_list_store_clear(bmList);
	gtk_list_store_append(bmList,&iter);
	gtk_list_store_set(bmList,&iter,BMLABEL,"Home",BMPATH,getenv("HOME"),-1);
	gtk_list_store_append(bmList,&iter);
	sprintf(buffer,"%s/Desktop",getenv("HOME"));
	gtk_list_store_set(bmList,&iter,BMLABEL,"Desktop",BMPATH,buffer,-1);
	gtk_list_store_append(bmList,&iter);
	gtk_list_store_set(bmList,&iter,BMLABEL,"Computer",BMPATH,"/",-1);

	asprintf(&filepath,"%s/.KKFileManager/bookmarks",getenv("HOME"));
	fp=fopen(filepath,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,PATH_MAX,fp))
				{
					if(strlen(buffer)>1)
						buffer[strlen(buffer)-1]=0;
					gtk_list_store_append(bmList,&iter);
					gtk_list_store_set(bmList,&iter,BMPATH,buffer,BMLABEL,basename(buffer),-1);
				}
			fclose(fp);
		}
	free(filepath);
}

void updateDiskList(void)
{
	char		*command;
	FILE		*fp=NULL;
	char		buffer[2048];
	GtkTreeIter	iter;
	char		*ptr;
	char		buffercommand[2048];
	char	*mountpath=NULL;
	char	*label=NULL;

	gtk_list_store_clear(diskList);
	asprintf(&command,"find /dev -maxdepth 1 -mindepth 1  -regextype sed -regex \"%s\"|grep -v \"%s\"|sort --version-sort",diskIncludePattern,diskExcludePattern);
//printf("command=%s\n",command);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						buffer[strlen(buffer)-1]=0;
						sprintf(buffercommand,"findmnt -no TARGET \"%s\"",buffer);
						mountpath=oneLiner(buffercommand);
						if(mountpath==NULL)
							mountpath=strdup("…");
						sprintf(buffercommand,"lsblk -no label \"%s\"",buffer);
						label=oneLiner(buffercommand);
						ptr=strrchr(buffer,'/');
						ptr++;
						gtk_list_store_append(diskList,&iter);
						gtk_list_store_set(diskList,&iter,DEVPATH,ptr,DISKNAME,label,MOUNTPATH,mountpath,MOUNTED,true,-1);
				}
			pclose(fp);
		}
	free(command);
}

void buildMenus(void)
{
	GtkWidget		*menu;

	menuBar=gtk_menu_bar_new();

//file menu
	fileMenu=gtk_menu_item_new_with_label("_File");
	gtk_menu_item_set_use_underline((GtkMenuItem*)fileMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMenu),menu);
//new
	menuItemNew=newMenuItem(MAINFILENEW,menu);
//terminals
	menuItemTerminal=newImageMenuItem(MAINFILETERMINAL,menu);

//prefs
	menuItemPrefs=newMenuItem(MAINFILEPREFS,menu);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),gtk_separator_menu_item_new());
//quit
	menuItemNew=newMenuItem(MAINFILEQUIT,menu);

	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),fileMenu);
}

void buidMainGui(const char *startdir)
{
	GtkWidget			*scrollbox;
	GtkCellRenderer		*renderer;
	GtkWidget			*label;
	GtkWidget			*labelbox;

	mainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(mainWindow),windowWidth,windowHeight);
	if(windowX!=-1 && windowY!=-1)
		gtk_window_move((GtkWindow *)mainWindow,windowX,windowY);
	g_signal_connect(G_OBJECT(mainWindow),"delete-event",G_CALLBACK(doShutdown),NULL);
	accgroup=gtk_accel_group_new();
	gtk_window_add_accel_group((GtkWindow*)mainWindow,accgroup);

	setUpContextMenus();

	mainNotebook=(GtkNotebook*)gtk_notebook_new();
	g_signal_connect(G_OBJECT(mainNotebook),"switch-page",G_CALLBACK(switchPage),NULL);

	mainVBox=createNewBox(NEWVBOX,false,0);

//main menus
//add menubar
	buildMenus();
	gtk_box_pack_start((GtkBox*)mainVBox,menuBar,false,false,0);

//add toolbar
	toolBar=(GtkToolbar*)gtk_toolbar_new();
	setUpToolBar();
	toolBarBox=createNewBox(NEWHBOX,true,0);
	gtk_box_pack_start(GTK_BOX(toolBarBox),(GtkWidget*)toolBar,true,true,0);
	gtk_box_pack_start(GTK_BOX(mainVBox),(GtkWidget*)toolBarBox,false,false,0);

#ifndef _USEGTK3_
	mainHPane=gtk_hpaned_new();
#else
	mainHPane=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
#endif
	leftVBox=createNewBox(NEWVBOX,false,0);
//left box
	gtk_paned_add1((GtkPaned*)mainHPane,leftVBox);
#ifndef _USEGTK3_
	leftVPane=gtk_vpaned_new();
#else
	leftVPane=gtk_paned_new(GTK_ORIENTATION_VERTICAL);
#endif
//disk list
	scrollbox=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)scrollbox,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	label=gtk_label_new("");
	gtk_label_set_markup((GtkLabel*)label,"<b>DISKS</b>");
	labelbox=createNewBox(NEWVBOX,false,0);
	gtk_box_pack_start(GTK_BOX(labelbox),label,false,false,0);

	gtk_box_pack_start(GTK_BOX(labelbox),(GtkWidget*)scrollbox,true,true,0);

	gtk_paned_add1((GtkPaned*)leftVPane,labelbox);
	gtk_box_pack_start(GTK_BOX(leftVBox),leftVPane,true,true,2);

	diskList=gtk_list_store_new(NUMDISKCOLS,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_BOOLEAN);
	diskView=(GtkTreeView*)gtk_tree_view_new_with_model((GtkTreeModel*)diskList);
	gtk_tree_view_set_headers_visible(diskView,false);
	g_signal_connect(diskView,"row-activated",G_CALLBACK(openDisk),NULL);	
	g_signal_connect(diskView,"button-press-event",G_CALLBACK(buttonDownDisk),NULL);	
//dev num
	renderer=gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes((GtkTreeView*)diskView,-1,"dev",renderer,"text",DEVPATH,NULL);
//label
	renderer=gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes((GtkTreeView*)diskView,-1,"label",renderer,"text",DISKNAME,NULL);

//mountpoint
	renderer=gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes((GtkTreeView*)diskView,-1,"mountpoint",renderer,"text",MOUNTPATH,NULL);

	updateDiskList();

	gtk_container_add(GTK_CONTAINER(scrollbox),(GtkWidget*)diskView);

//bookmark list
	scrollbox=gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow*)scrollbox,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	label=gtk_label_new("");
	gtk_label_set_markup((GtkLabel*)label,"<b>PLACES</b>");
	labelbox=createNewBox(NEWVBOX,false,0);
	gtk_box_pack_start(GTK_BOX(labelbox),label,false,false,0);

	gtk_box_pack_start(GTK_BOX(labelbox),(GtkWidget*)scrollbox,true,true,0);
	gtk_paned_add2((GtkPaned*)leftVPane,labelbox);

	bmList=gtk_list_store_new(NUMBMS,G_TYPE_STRING,G_TYPE_STRING);
	bmView=(GtkTreeView*)gtk_tree_view_new_with_model((GtkTreeModel*)bmList);
	gtk_tree_view_set_headers_visible(bmView,false);
	g_signal_connect(bmView,"row-activated",G_CALLBACK(openBM),NULL);	
	g_signal_connect(bmView,"button-press-event",G_CALLBACK(buttonDownBM),NULL);	
//bm label
	renderer=gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes((GtkTreeView*)bmView,-1,"label",renderer,"text",BMLABEL,NULL);

	updateBMList();
	gtk_container_add(GTK_CONTAINER(scrollbox),(GtkWidget*)bmView);

	gtk_widget_show_all((GtkWidget*)mainHPane);

//notbook
	gtk_paned_add2((GtkPaned*)mainHPane,(GtkWidget*)mainNotebook);
	gtk_paned_set_position((GtkPaned*)mainHPane,275);

	gtk_box_pack_start(GTK_BOX(mainVBox),(GtkWidget*)mainHPane,true,true,0);

	gtk_container_add((GtkContainer*)mainWindow,mainVBox);

	if(openDefault==true)
		addNewPage(getenv("HOME"));

//set left pane width
	gtk_paned_set_position((GtkPaned*)mainHPane,leftPaneWidth);
//set disk hite
	gtk_paned_set_position((GtkPaned*)leftVPane,leftPaneHeight);
	
	gtk_widget_show_all(mainWindow);
}

void updateTabLabel(pageStruct	*page)
{
	char	*correctedname;
	char	*basename;

	basename=g_path_get_basename(page->thisFolder);
	correctedname=truncateWithElipses(basename,maxTabChars);
	gtk_label_set_text(page->tabLabel,correctedname);
	gtk_widget_show_all((GtkWidget*)page->tabLabel);
	free(correctedname);
	free(basename);
}
