/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:16:28 GMT 2016 keithdhedger@gmail.com

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
#include <dirent.h>
#include <ctype.h>

#include "globals.h"

GdkPixbuf		*pixbuft;
const char		*iconNames[]={"user-home","user-desktop","computer","user-bookmarks","drive-removable-media-usb","drive-harddisk","media-optical","gnome-dev-disc-dvdrom","gnome-dev-cdrom","network-workgroup"};

GtkTargetEntry dragTargets[]={{(char*)"text/uri-list",0,DRAG_TEXT_URI_LIST}};
GtkTargetEntry dropTargets[]={{(char*)"text/uri-list",0,DROP_TEXT_URI_LIST},{(char*)"text/plain",0,DROP_TEXT_PLAIN}};

int				statusID;

menuDataStruct	menuData[]=
	{
//context
		{"New File",GTK_STOCK_NEW,0,0,NULL,"newfilemenu",NULL},
		{"New Folder",GTK_STOCK_DIRECTORY,0,0,NULL,"newfoldermenu",NULL},
		{"New Bookmark",GTK_STOCK_NEW,0,0,NULL,"newbmmenu",NULL},
		{"Open",GTK_STOCK_OPEN,0,0,NULL,"openmenu",NULL},
		{"Rename",GTK_STOCK_SAVE_AS,0,0,NULL,"renamemenu",NULL},
		{"Delete",GTK_STOCK_DELETE,0,0,NULL,"deletemenu",NULL},
		{"Duplicate",GTK_STOCK_COPY,0,0,NULL,"duplicatemenu",NULL},
		{"Extract",GTK_STOCK_CONVERT,0,0,NULL,"extractmenu",NULL},
		{"Copy",GTK_STOCK_COPY,0,0,NULL,"copymenu",NULL},
		{"Properties",GTK_STOCK_PROPERTIES,0,0,NULL,"propsmenu",NULL},

		{"Paste",GTK_STOCK_PASTE,0,0,NULL,"pastemenu",NULL},
		{"Show 'Dot' Files",GTK_STOCK_ZOOM_100,0,0,NULL,"hidemenu",NULL},
		{"Hide 'Dot' Files",GTK_STOCK_ZOOM_100,0,0,NULL,"showmenu",NULL},
//disk context
		{"Mount",GTK_STOCK_HARDDISK,0,0,NULL,"mountdiskmenu",NULL},
		{"Un-Mount",GTK_STOCK_HARDDISK,0,0,NULL,"unmountdiskmenu",NULL},
		{"Eject",GTK_STOCK_HARDDISK,0,0,NULL,"ejectdiskmenu",NULL},
//bm context
		{"Open",GTK_STOCK_OPEN,0,0,NULL,"openbmmenu",NULL},
		{"Delete",GTK_STOCK_DELETE,0,0,NULL,"deletebmmenu",NULL},

//main
//file
		{"New Tab",GTK_STOCK_NEW,0,0,(void*)&goNew,"newtabmenu",NULL},
		{"Terminal",GTK_STOCK_EXECUTE,0,0,(void*)&runTerminalHere,"terminalmenu",NULL},
		{"Preferences",GTK_STOCK_PREFERENCES,0,0,(void*)&doPrefs,"prefsmenu",NULL},
		{"Quit",GTK_STOCK_QUIT,0,0,(void*)&doShutdown,"quitmenu",NULL},
//help
		{"About",GTK_STOCK_ABOUT,0,0,(void*)&doAbout,"aboutmenu",NULL},
//tools and templates
//blank tool menu
		{"",NULL,0,0,NULL,NULL,NULL}
	};

contextStruct	**contextMenus;
GtkTargetEntry	*target=NULL;
int				romCnt=0;
int				diskCnt=0;
char			keyStrBuffer[32];

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
			gtk_image_set_pixel_size((GtkImage*)image,16);
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
			gtk_image_set_pixel_size ((GtkImage*)image,16);
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

void setNewPagePixbuf(GdkPixbuf *pixbuf,const char *type,const char *path,bool isdir,const char *mimetype,pageStruct *page)
{
	GtkTreeIter iter;

	gtk_list_store_append(page->listStore,&iter);
	if(pixbuf==NULL)
		pixbuf=genericText;
	gtk_list_store_set(page->listStore,&iter,PIXBUF_COLUMN,pixbuf,FILENAME,type,FILEPATH,path,ISDIR,isdir,MIMETYPE,mimetype,-1);
}

char *getMimeType(const char *path)
{
	return(strdup((char*)mime_type_get_by_file(path,NULL,NULL)));
}

unsigned hashMimeType(char* cp)
{
	unsigned hash=5381;
	while(*cp)
		hash=33*hash^*cp++;
	return(hash);
}

GdkPixbuf* getPixBuf(const char *file,const char *mimetype)
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

	if(mimetype==NULL)
		mime=getMimeType(file);
	else
		mime=strdup(mimetype);

	//mime=strdup(mimetype);
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

	if(strcmp(mime,"inode/directory")==0)
		{
		//hack for mising icons
			char *folderpath=NULL;
			folderpath=getDIcon("inode/directory","folder");
			if(folderpath==NULL)
				folderpath=getDIcon("inode/directory","gnome-fs-directory");
			if(folderpath==NULL)
				folderpath=getDIcon("inode/directory","gtk-directory");
			if(folderpath==NULL)
				pb=genericText;
			else
				pb=gdk_pixbuf_new_from_file_at_size(folderpath,-1,iconSize,NULL);
			if(folderpath!=NULL)
				free(folderpath);
		}
	else
		{
			icon=g_content_type_get_icon(mime);
			info=gtk_icon_theme_lookup_by_gicon(defaultTheme,icon,iconSize,(GtkIconLookupFlags)0);
			if(info==NULL)
				pb=genericText;
			else
				pb=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,iconSize,NULL);
		}

	if(issymlink==true)
		gdk_pixbuf_composite(symLink,pb,iconSize-16,iconSize-16,16,16,iconSize-16,iconSize-16,1.0,1.0,GDK_INTERP_NEAREST,255);

	if(isbrokenlink==true)
		gdk_pixbuf_composite(brokenLink,pb,0,iconSize-16,16,16,0,iconSize-16,1.0,1.0,GDK_INTERP_NEAREST,255);

	pixBuffCache[hash]=pb;
	free(mime);
	if(icon!=NULL)
		g_object_unref(icon);
	if(info!=NULL)
#ifndef _USEGTK3_
		gtk_icon_info_free(info);
#else
		g_object_unref(info);
#endif
	return(pb);
}

inline bool ignoredots(const char *ptr)
{
	if(ptr==NULL)
		return(false);

	if(ptr[0]==0)
		return(false);

	if((ptr[1]==0) && (ptr[0]=='.'))
		return(false);

	if((ptr[0]=='.') && (ptr[1]=='.') && (ptr[2]==0))
		return(false);

	return(true);

}

gboolean loadThumbnailsGtk3(gpointer arg)
{
	char		pathasstring[16];
	GtkTreeIter	searchiter;
	char		*filepath=NULL;
	GdkPixbuf	*pixbuf;
	char		*mime=NULL;
	bool		gotiter;
	pageStruct	*page=(pageStruct*)arg;

	if(page->thumbnailThreadRunning==false)
		return(false);

	sprintf(pathasstring,"%i",page->fromHere);
	gotiter=gtk_tree_model_get_iter_from_string((GtkTreeModel*)page->listStore,&searchiter,pathasstring);
	if(gotiter==false)
		{
			page->thumbnailThreadRunning=false;
			return(false);
		}
	gtk_tree_model_get(GTK_TREE_MODEL(page->listStore),&searchiter,MIMETYPE,&mime,FILEPATH,&filepath,-1);
	if(g_str_has_prefix(mime,"image"))
		{
			pixbuf=gdk_pixbuf_new_from_file_at_size(filepath,-1,iconSize,NULL);											
			gtk_list_store_set(page->listStore,&searchiter,PIXBUF_COLUMN,pixbuf,-1);
			g_object_unref(pixbuf);
			free(filepath);
		}
	free(mime);

	page->fromHere++;
	if(page->uptoHere>=page->fileCnt)
		{
			page->fileType++;
			page->thumbnailThreadRunning=false;
			return(false);
		}
	return(true);
}

void* loadThumbnails(void *arg)
{
	char		pathasstring[16];
	GtkTreeIter	searchiter;
	char		*filepath=NULL;
	GdkPixbuf	*pixbuf;
	char		*mime=NULL;
	bool		gotiter;
	pageStruct	*page=(pageStruct*)arg;

	page->thumbnailThreadRunning=true;

	for(int j=0;j<page->fileCnt;j++)
		{
			sprintf(pathasstring,"%i",j);
			gotiter=gtk_tree_model_get_iter_from_string((GtkTreeModel*)page->listStore,&searchiter,pathasstring);
			if(gotiter==false)
				{
					page->thumbnailThreadRunning=false;
					return(NULL);
				}
			gtk_tree_model_get(GTK_TREE_MODEL(page->listStore),&searchiter,MIMETYPE,&mime,FILEPATH,&filepath,-1);
			if(g_str_has_prefix(mime,"image"))
				{
					pixbuf=gdk_pixbuf_new_from_file_at_size(filepath,-1,iconSize,NULL);											
					gtk_list_store_set(page->listStore,&searchiter,PIXBUF_COLUMN,pixbuf,-1);
					g_object_unref(pixbuf);
					free(filepath);
				}
			free(mime);
		}

	page->thumbnailThreadRunning=false;
	return(NULL);
}

gboolean loadFilesDir(gpointer data)
{
	GdkPixbuf	*pixbuf;
	char		buffer[PATH_MAX];
	pageStruct	*page=(pageStruct*)data;
	char		*mimetype=NULL;
	char		rootpath[PATH_MAX];
	struct		statfs;
 
	while(gtk_events_pending())
		gtk_main_iteration();

	if(strcmp(page->thisFolder,"/")==0)
		sprintf(rootpath,"%s","/");
	else
		sprintf(rootpath,"%s/",page->thisFolder);

	switch(page->fileType)
		{
			case LOADHIDENFOLDERS:
				if(showHidden==false)
					{
						page->fileType++;
						page->fromHere=0;
						page->uptoHere=LOADICONCNT;
						if(page->uptoHere > page->fileCnt)
							page->uptoHere=page->fileCnt;
						break;
					}

				for(int j=page->fromHere;j<page->uptoHere;j++)
					{
						if(page->fileList[j]->d_type==DT_DIR)
							if(page->fileList[j]->d_name[0]=='.')
								{
									sprintf(buffer,"%s%s",rootpath,page->fileList[j]->d_name);
									pixbuf=getPixBuf(buffer,"inode/directory");
									setNewPagePixbuf(pixbuf,page->fileList[j]->d_name,buffer,true,"inode/directory",page);
								}
					}
				if(page->uptoHere==page->fileCnt)
					{
						page->fileType++;
						page->fromHere=0;
						page->uptoHere=LOADICONCNT;
						if(page->uptoHere > page->fileCnt)
							page->uptoHere=page->fileCnt;
					}
				else
					{
						page->fromHere=page->uptoHere;
						page->uptoHere=page->uptoHere+LOADICONCNT;
						if(page->uptoHere > page->fileCnt)
							page->uptoHere=page->fileCnt;
					}
				break;

			case LOADFOLDERs:
				for(int j=page->fromHere;j<page->uptoHere;j++)
					{
						if(page->fileList[j]->d_type==DT_DIR)
							if(page->fileList[j]->d_name[0]!='.')
								{
									sprintf(buffer,"%s%s",rootpath,page->fileList[j]->d_name);
									pixbuf=getPixBuf(buffer,"inode/directory");
									setNewPagePixbuf(pixbuf,page->fileList[j]->d_name,buffer,true,"inode/directory",page);
								}
					}
				if(page->uptoHere==page->fileCnt)
					{
						page->fileType++;
						page->fromHere=0;
						page->uptoHere=LOADICONCNT;
						if(page->uptoHere > page->fileCnt)
							page->uptoHere=page->fileCnt;
					}
				else
					{
						page->fromHere=page->uptoHere;
						page->uptoHere=page->uptoHere+LOADICONCNT;
						if(page->uptoHere > page->fileCnt)
							page->uptoHere=page->fileCnt;
					}
			break;

			case LOADHIDDENFILES:
				if(showHidden==false)
					{
						page->fileType++;
						page->fromHere=0;
						page->uptoHere=LOADICONCNT;
						if(page->uptoHere > page->fileCnt)
							page->uptoHere=page->fileCnt;
						return(true);
						break;
					}
				for(int j=page->fromHere;j<page->uptoHere;j++)
					{
						if(page->fileList[j]->d_type!=DT_DIR)
							if(page->fileList[j]->d_name[0]=='.')
								{
									sprintf(buffer,"%s%s",rootpath,page->fileList[j]->d_name);
									mimetype=getMimeType(buffer);
									pixbuf=getPixBuf(buffer,mimetype);
									setNewPagePixbuf(pixbuf,page->fileList[j]->d_name,buffer,false,mimetype,page);
									free(mimetype);
								}
					}
				if(page->uptoHere==page->fileCnt)
					{
						page->fileType++;
						page->fromHere=0;
						page->uptoHere=LOADICONCNT;
						if(page->uptoHere >= page->fileCnt)
							page->uptoHere=page->fileCnt;
					}
				else
					{
						page->fromHere=page->uptoHere;
						page->uptoHere=page->uptoHere+LOADICONCNT;
						if(page->uptoHere > page->fileCnt)
							page->uptoHere=page->fileCnt;
					}
				break;

			case LOADFILES:
				for(int j=page->fromHere;j<page->uptoHere;j++)
					{
						if(page->fileList[j]->d_type!=DT_DIR)
							if(page->fileList[j]->d_name[0]!='.')
								{
									sprintf(buffer,"%s%s",rootpath,page->fileList[j]->d_name);
									mimetype=getMimeType(buffer);
									pixbuf=getPixBuf(buffer,mimetype);
									setNewPagePixbuf(pixbuf,page->fileList[j]->d_name,buffer,false,mimetype,page);
									free(mimetype);
								}
					}

				if(page->uptoHere==page->fileCnt)
					{
						page->fileType++;
						if(noThumbs==true)
							{
								sprintf(buffer,"stat --file-system \"%s\"|grep Type:|awk -F'Type: ' '{print $2}'",rootpath);
								char *type=oneLiner(buffer,NULL);
								char	**ignoretypes;
								int		cnt=0;
								ignoretypes=g_strsplit(noNetPicsOn,";",-1);
								if(ignoretypes!=NULL)
									{
										while(ignoretypes[cnt]!=NULL)
											{
												if(strcmp(type,ignoretypes[cnt])==0)
													{
														page->fileType=0;
														return(false);
													}
												cnt++;
											}
									}
								free(type);
							}
					}
				else
					{
						page->fromHere=page->uptoHere;
						page->uptoHere=page->uptoHere+LOADICONCNT;
						if(page->uptoHere > page->fileCnt)
							page->uptoHere=page->fileCnt;
					}
				break;

			case LOADPIXMAPS:
			page->fromHere=0;
#ifdef _USEGTK3_
			page->uptoHere=LOADTHUMBCNT;
			if(page->uptoHere > page->fileCnt)
				page->uptoHere=page->fileCnt;
			page->thumbnailThreadRunning=true;
			gdk_threads_add_timeout(250,loadThumbnailsGtk3,(gpointer)page);
#else
				sinkReturn=pthread_create(&(page->thumbnailThread),NULL,&loadThumbnails,(void*)page);
#endif
				page->fileType++;
				return(false);
				break;
		}

	return(true);
}

int filter(const struct dirent *entry1)
{
	struct stat	st;

	if(strcmp(entry1->d_name,".")==0 || strcmp(entry1->d_name,"..")==0)
		return(false);

	stat(entry1->d_name,&st);
	struct dirent *e1=(struct dirent *)entry1;

	if((st.st_mode & S_IFMT)==S_IFDIR)
		e1->d_type=DT_DIR;

	return(true);
}

void populatePageStore(pageStruct *page)
{
	char	*cwd=NULL;

	if(page==NULL)
		return;

	if(page->thumbnailThreadRunning==true)
		{
#ifndef _USEGTK3_
			pthread_cancel(page->thumbnailThread);
#endif
			page->thumbnailThreadRunning=false;
		}

	flushFolderBuffer(page);
	gtk_list_store_clear(page->listStore);

	cwd=get_current_dir_name();
	chdir(page->thisFolder);
	printf("file cnt in=%i\n",page->fileCnt);
	page->fileCnt=scandir(page->thisFolder,&page->fileList,filter,versionsort);
	printf("file cnt out=%i\n",page->fileCnt);
	page->fileType=0;
	page->fromHere=0;
	page->uptoHere=LOADICONCNT;
	if(page->uptoHere >= page->fileCnt)
		page->uptoHere=page->fileCnt;

	if(cwd!=NULL)
		chdir(cwd);

	//gdk_threads_add_idle(loadFilesDir,(gpointer)page);

//	g_timeout_add(10,loadFilesDir,(gpointer)page);
	gdk_threads_add_timeout(200,loadFilesDir,(gpointer)page);
//	gdk_threads_add_idle(loadFilesDir,(gpointer)page);
	gtk_widget_show_all((GtkWidget*)page->scrollBox);
}

bool	resetSearch=false;

gboolean keyIcon(GtkWidget *widget,GdkEventKey *event,pageStruct *page)
{
	bool		downkey=false;
	char		temp[16];
	char		pathasstring[16];
	char		*filename;
	GtkTreeIter	searchiter;
	GtkTreePath	*treepath=NULL;
	bool		loopup=false;
	bool		loop=true;
	int			maxitems;
	int			cntloops;

	temp[0]=0;
	sprintf(pathasstring,"%i",page->searchPath);
	treepath=gtk_tree_path_new_from_string(pathasstring);
	maxitems=gtk_tree_model_iter_n_children((GtkTreeModel*)page->listStore,NULL)-1;
	if(maxitems<1)
		return(false);

/*
struct	contextStruct
{
	pageStruct	*page;
	GtkTreePath	*treepath;
	GtkTreeView *tree;
	unsigned	id;
};

*/
	switch(event->keyval)
		{
			case GDK_KEY_Delete:
				{
					contextStruct ctx={page,treepath,(GtkTreeView*)page->iconView,CONTEXTDELETE};
					contextMenuActivate(NULL,&ctx);
				}
				break;
			case GDK_KEY_BackSpace:
				page->searchString[strlen(page->searchString)-1]=0;
				buildMessgage(page);
				return(true);
				break;

			case GDK_KEY_Down:
				if(strlen(page->searchString)>0)
					{
						resetSearch=true;
						downkey=true;
						loopup=false;
						page->searchPath++;
						if(page->searchPath>maxitems)
							page->searchPath=0;
						buildMessgage(page);
					}
				else
					{
						buildMessgage(page);
						return(false);
					}
				break;

			case GDK_KEY_Up:
				if(strlen(page->searchString)>0)
					{
						resetSearch=true;
						loopup=true;
						downkey=true;
						page->searchPath--;
						if(page->searchPath<0)
							page->searchPath=maxitems;
						buildMessgage(page);
					}
				else
					{
						buildMessgage(page);
						return(false);
					}
				break;

			case GDK_KEY_Return:
				if(strlen(page->searchString)>0)
					{
						selectItem(page->iconView,treepath,page);
						gtk_tree_path_free(treepath);
						return(true);
					}
				else
					return(false);
				break;

			default:
				if(resetSearch==true)
					{
						page->searchString[0]=0;
						resetSearch=false;
					}
				 if((isalnum(event->keyval)) || (ispunct(event->keyval)))
				 	{
						sprintf(&temp[0],"%c",event->keyval);
						strcat(page->searchString,temp);
						buildMessgage(page);
					}
				else
					return(false);
				break;
		}

	cntloops=0;
	do
		{
			filename=NULL;
			sprintf(pathasstring,"%i",page->searchPath);
			treepath=gtk_tree_path_new_from_string(pathasstring);
			if(treepath==NULL)
				return(downkey);
	
			if(gtk_tree_model_get_iter_from_string((GtkTreeModel*)page->listStore,&searchiter,pathasstring)==true)
				{
					gtk_tree_model_get(GTK_TREE_MODEL(page->listStore),&searchiter,FILENAME,&filename,-1);
					if(filename!=NULL)
						{
							if(strcasestr(filename,page->searchString)!=NULL)
								{
									gtk_icon_view_unselect_all(page->iconView);
									gtk_icon_view_select_path(page->iconView,treepath);
									gtk_icon_view_scroll_to_path(page->iconView,treepath,false,0.0,0.0);
									gtk_tree_path_free(treepath);
									buildMessgage(page);
									free(filename);
									return(true);
								}
						}
				if(loopup==false)
					{
						page->searchPath++;
						if(page->searchPath>maxitems)
							page->searchPath=0;
					}
				else
					{
						page->searchPath--;
						if(page->searchPath<0)
							page->searchPath=maxitems;
					}
				}
			gtk_tree_path_free(treepath);
			cntloops++;
			if(cntloops>maxitems)
				loop=false;
		}
	while(loop);
	return(downkey);
}

void renameItem(GtkCellRendererText *cell,gchar *path_string,gchar *text,pageStruct *page)
{
	char			buffer[PATH_MAX+20];
	GtkTreeIter		iter;
	GtkTreePath		*path;

	filePathStruct	fps={NULL,NULL,NULL,NULL,NULL,NULL,false,false,false,false};

	if(page==NULL)
		return;

	path=gtk_tree_path_new_from_string(path_string);
	gtk_tree_model_get_iter(GTK_TREE_MODEL(page->listStore),&iter,path);
	gtk_tree_model_get(GTK_TREE_MODEL(page->listStore),&iter,FILEPATH,&fps.fromFilePath,-1);
	
	setFilePathStruct(&fps,"NDTpt",text,page->thisFolder);

	if(strcmp(fps.fromFilePath,fps.toFilePath)==0)
		{
			fps.askFileName=true;
			getValidToPathFromFilepath(&fps);
			if(fps.modified==false)
				{
					freefilePathStruct(&fps,false);
					gtk_tree_path_free(path);
					return;
				}
		}

	sprintf(buffer,"mv \"%s\" \"%s\"\n",fps.fromFilePath,fps.toFilePath);
	system(buffer);

	freefilePathStruct(&fps,false);
	gtk_tree_path_free(path);
	setCurrentFolderForTab(page->thisFolder,page,true,true);
}

void newIconView(pageStruct *page)
{
	GtkCellRenderer	*renderer;
	float			align;
	char			buffer[64];
	
	sprintf(buffer,"GtkIconView-%u",pageCnt);
//TODO//
//gtk_tree_view_column_set_min_width
//GtkTreeSelection  *selection;
//GtkCellRendererText
// gtk_tree_view_column
	page->listStore=gtk_list_store_new(NUMCOLS,G_TYPE_STRING,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_BOOLEAN,G_TYPE_STRING);
	page->iconView=(GtkIconView*)gtk_icon_view_new();
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(page->iconView),PIXBUF_COLUMN);
	//gtk_icon_view_set_text_column(GTK_ICON_VIEW(page->iconView),FILENAME);

	renderer=gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_end(GTK_CELL_LAYOUT(page->iconView),renderer,false);
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (page->iconView),renderer,"text",FILENAME,NULL);

#ifdef _USEGTK3_
	align=0.5;
#else
	align=0.0;
#endif

	g_object_set(renderer,"alignment",PANGO_ALIGN_CENTER,"wrap-mode",PANGO_WRAP_WORD_CHAR,"xalign",align,"yalign",align,"editable",true,"wrap-width",iconSize*2,NULL);
	g_signal_connect(renderer,"edited",G_CALLBACK(renameItem),page);

	gtk_icon_view_set_model(GTK_ICON_VIEW(page->iconView),GTK_TREE_MODEL(page->listStore));
	gtk_icon_view_set_item_padding(GTK_ICON_VIEW(page->iconView),0);

	page->bdownsignal=g_signal_connect(page->iconView,"button-press-event",G_CALLBACK(buttonDown),page);	
	page->bupsignal=g_signal_connect(page->iconView,"button-release-event",G_CALLBACK(buttonUp),page);	
	page->selectsignal=g_signal_connect_after(page->iconView,"item-activated",G_CALLBACK(selectItem),page);	
//	g_signal_connect(page->iconView,"selection-changed",G_CALLBACK(rochanged),page);	
	g_signal_connect(page->iconView,"key-press-event",G_CALLBACK(keyIcon),page);	
	g_signal_connect(page->iconView,"selection-changed",G_CALLBACK(buildMessgage),page);	
//	page->bupsignal=g_signal_connect(page->iconView,"button-release-event",G_CALLBACK(buildMessgage),page);	

	populatePageStore(page);

	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(page->iconView),GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_item_orientation(GTK_ICON_VIEW(page->iconView),GTK_ORIENTATION_VERTICAL);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(page->iconView),-1);

	target=(GtkTargetEntry*)calloc(1,sizeof(GtkTargetEntry));
	target->target=(gchar*)"InternalTarget";
	target->flags=GTK_TARGET_SAME_APP;
	target->info=0;
	gtk_icon_view_enable_model_drag_source(page->iconView,GDK_BUTTON1_MASK,dragTargets,1,(GdkDragAction)(GDK_ACTION_COPY|GDK_ACTION_MOVE|GDK_ACTION_LINK));
	gtk_icon_view_enable_model_drag_dest(page->iconView,dropTargets,2,(GdkDragAction)(GDK_ACTION_COPY|GDK_ACTION_MOVE|GDK_ACTION_LINK));

//drag
	g_signal_connect(G_OBJECT(page->iconView),"drag-begin",G_CALLBACK(doDragBegin),(gpointer)page);
	g_signal_connect(G_OBJECT(page->iconView),"drag-data-get",G_CALLBACK(dragDataGet),(gpointer)page);
	g_signal_connect(G_OBJECT(page->iconView),"drag-end",G_CALLBACK(doDragEnd),(gpointer)page);
//drop
	g_signal_connect(G_OBJECT(page->iconView),"drag-drop",G_CALLBACK(dragDrop),(gpointer)page);
	g_signal_connect(G_OBJECT(page->iconView),"drag-data-received",G_CALLBACK(dragDataReceived),(gpointer)page);
	gtk_icon_view_set_column_spacing(page->iconView,iconPadding);
	gtk_icon_view_set_item_width (page->iconView,iconSize*iconSize3);

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
					return(retstr);
				}
		}
	return(strdup(str));
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

	gtk_box_pack_start(GTK_BOX(hbox),(GtkWidget*)gtk_label_new(""),true,true,0);

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
	gtk_list_store_set(bmList,&iter,BMPIXBUF,guiPixbufs[HOMEPB],BMLABEL,"Home",BMPATH,getenv("HOME"),-1);
	gtk_list_store_append(bmList,&iter);
	sprintf(buffer,"%s/Desktop",getenv("HOME"));
	gtk_list_store_set(bmList,&iter,BMPIXBUF,guiPixbufs[DESKTOPPB],BMLABEL,"Desktop",BMPATH,buffer,-1);
	gtk_list_store_append(bmList,&iter);
	gtk_list_store_set(bmList,&iter,BMPIXBUF,guiPixbufs[COMPUTERPB],BMLABEL,"Computer",BMPATH,"/",-1);

	asprintf(&filepath,"%s/.KKFileManager/bookmarks",getenv("HOME"));
	fp=fopen(filepath,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,PATH_MAX,fp))
				{
					if(strlen(buffer)>1)
						buffer[strlen(buffer)-1]=0;
					gtk_list_store_append(bmList,&iter);
					gtk_list_store_set(bmList,&iter,BMPIXBUF,guiPixbufs[BOOKMARKPB],BMPATH,buffer,BMLABEL,basename(buffer),-1);
				}
			fclose(fp);
		}
	free(filepath);
}

bool checkDisksChanged(void)
{
	char	*command;
	int		cnt=0;;
	
	command=oneLiner("ls /dev/disk/by-path -1|wc -l",NULL);
	cnt=atoi(command);
	free(command);
	if(cnt!=diskCnt)
		{
			diskCnt=cnt;
			return(true);
		}
	diskCnt=cnt;
	return(false);
}

bool checkCDROMChanged(void)
{
	char		*command;
	FILE		*fp=NULL;
	char		buffer[2048];
	char		buffercommand[2048];
	char		*isdvd=NULL;
	int			mult=1;
	int			last=romCnt;

	romCnt=0;
	asprintf(&command,"find /dev -maxdepth 1 -mindepth 1  -regextype sed -regex \"%s\"|grep -v \"%s\"|sort --version-sort",diskIncludePattern,diskExcludePattern);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						buffer[strlen(buffer)-1]=0;

					sprintf(buffercommand,"udevadm info --name=\"%s\"|grep ID_CDROM_MEDIA_",buffer);
					isdvd=oneLiner(buffercommand,NULL);
					if(isdvd!=NULL)
						{
							romCnt=romCnt+(mult*1);
							mult*=10;
							free(isdvd);
							isdvd=NULL;
						}
				}
			pclose(fp);
		}
	if(last!=romCnt)
		return(true);
	return(false);
}

void updateDiskList(void)
{
	char				*command;
	FILE				*fp=NULL;
	char				buffer[2048];
	GtkTreeIter			iter;
	char				*ptr;
	char				buffercommand[2048];
	char				*mountpath=NULL;
	char				*label=NULL;
	char				*isusb=NULL;
	char				*isdvd=NULL;
	GdkPixbuf			*drive=NULL;
	bool				gotrom=false;
	networkDriveStruct	*netmount=(networkDriveStruct*)alloca(sizeof(networkDriveStruct));
	netmount->url=NULL;
	netmount->fstype=NULL;
	netmount->host=NULL;
	netmount->port=NULL;
	netmount->user=NULL;
	netmount->pass=NULL;
	netmount->path=NULL;

	gtk_list_store_clear(diskList);

	asprintf(&command,"cat /proc/mounts|awk '{print $1 \" \" $2}'");
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			gchar	**array=NULL;
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						{
							buffer[strlen(buffer)-1]=0;
							array=g_strsplit(buffer," ",-1);
							if(array!=NULL)
								{
								//printf(">>>%s<<<\n",array[0]);
									netmount->fstype=NULL;
									netmount->host=NULL;
									netmount->path=NULL;
									if(parseNetworkUrl(array[0],netmount)==VALIDURL)
										{
										//printf(">>%s<<\n",netmount->fsType);
										//printf(">>%s<<\n",netmount->serverName);
										//printf(">>%s<<\n",netmount->sharePath);
										//printf("-----------------------------\n");
											gtk_list_store_append(diskList,&iter);
											gtk_list_store_set(diskList,&iter,DEVPIXBUF,guiPixbufs[NETWORKDIVE],DEVPATH,netmount->host,DISKNAME,netmount->path,MOUNTPATH,array[1],MOUNTED,true,-1);
										}
									g_strfreev(array);
								}
						}
				}
			pclose(fp);
		}
	clearNMStruct(netmount);

	asprintf(&command,"find /dev -maxdepth 1 -mindepth 1  -regextype sed -regex \"%s\"|grep -v \"%s\"|sort --version-sort",diskIncludePattern,diskExcludePattern);
//printf("command=%s\n",command);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,2048,fp))
				{
					if(strlen(buffer)>0)
						{
							buffer[strlen(buffer)-1]=0;
							sprintf(buffercommand,"findmnt -no TARGET \"%s\"",buffer);
							mountpath=oneLiner(buffercommand,NULL);
							if(mountpath==NULL)
								mountpath=strdup("…");
							sprintf(buffercommand,"lsblk -no label \"%s\"",buffer);
							label=oneLiner(buffercommand,NULL);

							ptr=strrchr(buffer,'/');
							ptr++;
						
//generic disk
							drive=guiPixbufs[HDDRIVEPB];
//usb disk
							sprintf(buffercommand,"udevadm info --query=all --name=\"%s\" |grep -i usb",ptr);
							isusb=oneLiner(buffercommand,NULL);
							if((isusb!=NULL) && (strlen(isusb)>0))
								{
									drive=guiPixbufs[USBDISKPB];
									free(isusb);
								}
//dvd disk
							sprintf(buffercommand,"udevadm info --name=\"%s\"|grep ID_CDROM",ptr);
							isdvd=oneLiner(buffercommand,NULL);
							if(isdvd!=NULL)
								{
									free(isdvd);
									isdvd=NULL;
									gotrom=false;
									sprintf(buffercommand,"udevadm info --name=\"%s\"|grep ID_CDROM_MEDIA_DVD",ptr);
									isdvd=oneLiner(buffercommand,NULL);
									if((isdvd!=NULL) && (strlen(isdvd)>0))
										{
											drive=guiPixbufs[DVDROMPB];
											gotrom=true;
											free(isdvd);
										}
//cdrom
									sprintf(buffercommand,"udevadm info --name=\"%s\"|grep ID_CDROM_MEDIA_CD",ptr);
									isdvd=oneLiner(buffercommand,NULL);
									if((isdvd!=NULL) && (strlen(isdvd)>0))
										{
											drive=guiPixbufs[CDROMPB];
											gotrom=true;
											free(isdvd);
										}
									if(gotrom==true)
										{
											gtk_list_store_append(diskList,&iter);
											gtk_list_store_set(diskList,&iter,DEVPIXBUF,drive,DEVPATH,ptr,DISKNAME,label,MOUNTPATH,mountpath,MOUNTED,true,-1);
										}
								}
							else
								{
									gtk_list_store_append(diskList,&iter);
									gtk_list_store_set(diskList,&iter,DEVPIXBUF,drive,DEVPATH,ptr,DISKNAME,label,MOUNTPATH,mountpath,MOUNTED,true,-1);
								}
						}
				}
			pclose(fp);
		}
	free(command);
}

void destroyTool(gpointer data)
{
/*
	toolStruct	*
	if(((toolStruct*)data)->menuName!=NULL)
		{
			debugFree(&((toolStruct*)data)->menuName);
		}
	if(((toolStruct*)data)->filePath!=NULL)
		{
			debugFree(&((toolStruct*)data)->filePath);
		}
	if(((toolStruct*)data)->command!=NULL)
		{
			debugFree(&((toolStruct*)data)->command);
		}
	free((char**)&data);
*/
}

gint sortTools(gconstpointer a,gconstpointer b)
{
	return(strcasecmp(((toolStruct*)a)->menuName,((toolStruct*)b)->menuName));
}

void buildToolsList(void)
{
	GDir			*folder;
	const gchar		*entry=NULL;
	char			*filepath;
	toolStruct		*tool;
	char			*datafolder[2];

	if(toolsList!=NULL)
		{
			g_list_free_full(toolsList,destroyTool);
			toolsList=NULL;
		}

	sinkReturn=asprintf(&datafolder[0],"%s/",GTOOLSFOLDER);
	sinkReturn=asprintf(&datafolder[1],"%s/%s/tools/",getenv("HOME"),APPFOLDENAME);
	for(int loop=0; loop<2; loop++)
		{
			folder=g_dir_open(datafolder[loop],0,NULL);
			if(folder!=NULL)
				{
					entry=g_dir_read_name(folder);
					while(entry!=NULL)
						{
							intermarg=0;
							flagsarg=0;
							inpopup=0;
							inmenu=0;
							inboth=0;
							menuname=NULL;
							commandarg=NULL;

							sinkReturn=asprintf(&filepath,"%s%s",datafolder[loop],entry);
							loadVarsFromFile(filepath,tool_vars);

							if((menuname!=NULL) &&(strlen(menuname)>0) &&(commandarg!=NULL))
								{
									tool=(toolStruct*)malloc(sizeof(toolStruct));
									tool->menuName=strdup(menuname);
									tool->command=strdup(commandarg);
									tool->flags=flagsarg;
									tool->inTerminal=(bool)intermarg;
									tool->inPopUp=(bool)inpopup;
									tool->inMenu=(bool)inmenu;
									tool->inBoth=(bool)inboth;
									tool->filePath=strdup(filepath);

									if(loop==0)
										tool->global=true;
									else
										tool->global=false;

									toolsList=g_list_prepend(toolsList,(gpointer)tool);
									free(menuname);
									free(commandarg);
								}
							free(filepath);
							entry=g_dir_read_name(folder);
						}
				}
		}
	free(datafolder[0]);
	free(datafolder[1]);

	toolsList=g_list_sort(toolsList,sortTools);
}

void buildTemplates(void)
{
	GDir			*folder;
	const gchar		*entry=NULL;
	char			*templatefolder=(char*)alloca(PATH_MAX);
	GtkWidget		*submenu=gtk_menu_item_get_submenu((GtkMenuItem*)templateMenu);

	if(submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)templateMenu,NULL);

	templateSubMenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(templateMenu),templateSubMenu);

	snprintf(templatefolder,PATH_MAX,"%s/Templates/",getenv("HOME"));
	folder=g_dir_open(templatefolder,0,NULL);
	if(folder!=NULL)
		{
			entry=g_dir_read_name(folder);

			menuData[MAINTOOLSBLANKTOOL].cb=(void*)makeTemplate;
			menuData[MAINTOOLSBLANKTOOL].stockID=NULL;
			menuData[MAINTOOLSBLANKTOOL].key=0;
			menuData[MAINTOOLSBLANKTOOL].userData=NULL;

			while(entry!=NULL)
				{
					if(entry[0]!='.')
						{
							menuData[MAINTOOLSBLANKTOOL].menuLabel=entry;
							submenu=newMenuItem(MAINTOOLSBLANKTOOL,templateSubMenu);
						}
					entry=g_dir_read_name(folder);
				}
		}
}

void buildTools(void)
{
	GtkWidget		*menuitem;
	GList			*ptr;
	bool			gotglobal=false;

	buildToolsList();

	GtkWidget *submenu=gtk_menu_item_get_submenu((GtkMenuItem*)toolsMenu);
	if(submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)toolsMenu,NULL);

	toolsSubMenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(toolsMenu),toolsSubMenu);

//addtool
	ptr=toolsList;
	menuData[MAINTOOLSBLANKTOOL].cb=(void*)externalTool;
	menuData[MAINTOOLSBLANKTOOL].stockID=NULL;
	menuData[MAINTOOLSBLANKTOOL].key=0;
	while(ptr!=NULL)
		{
			if((((toolStruct*)ptr->data)->global==true) && ((((toolStruct*)ptr->data)->inMenu==true) || (((toolStruct*)ptr->data)->inBoth==true)))
				{
					gotglobal=true;
					menuData[MAINTOOLSBLANKTOOL].menuLabel=((toolStruct*)ptr->data)->menuName;
					menuData[MAINTOOLSBLANKTOOL].userData=(gpointer)ptr->data;
					menuitem=newMenuItem(MAINTOOLSBLANKTOOL,toolsSubMenu);
				}
			ptr=g_list_next(ptr);
		}

	if(gotglobal==true)
		{
			menuitem=gtk_separator_menu_item_new();
			gtk_menu_shell_append(GTK_MENU_SHELL(toolsSubMenu),menuitem);
		}

	ptr=toolsList;
	menuData[MAINTOOLSBLANKTOOL].cb=(void*)externalTool;
	menuData[MAINTOOLSBLANKTOOL].stockID=NULL;
	menuData[MAINTOOLSBLANKTOOL].key=0;

	while(ptr!=NULL)
		{
			if((((toolStruct*)ptr->data)->global==false) && ((((toolStruct*)ptr->data)->inMenu==true) || (((toolStruct*)ptr->data)->inBoth==true)))
				{
					menuData[MAINTOOLSBLANKTOOL].menuLabel=((toolStruct*)ptr->data)->menuName;
					menuData[MAINTOOLSBLANKTOOL].userData=(gpointer)ptr->data;
					menuitem=newMenuItem(MAINTOOLSBLANKTOOL,toolsSubMenu);
				}
			ptr=g_list_next(ptr);
		}
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

//templates
	templateMenu=gtk_menu_item_new_with_label("_Templates");
	gtk_menu_item_set_use_underline((GtkMenuItem*)templateMenu,true);
	buildTemplates();

//external tools
	toolsMenu=gtk_menu_item_new_with_label("_Tools");
	gtk_menu_item_set_use_underline((GtkMenuItem*)toolsMenu,true);
	buildTools();

//help
	helpMenu=gtk_menu_item_new_with_label("_Help");
	gtk_menu_item_set_use_underline((GtkMenuItem*)helpMenu,true);
	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpMenu),menu);
//about
	aboutMenu=newMenuItem(MAINHELPABOUT,menu);

	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),fileMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),templateMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),toolsMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBar),helpMenu);
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

	gtk_window_set_default_icon_name(PACKAGE);
	gtk_window_set_icon_name((GtkWindow*)mainWindow,PACKAGE);

	g_signal_connect(G_OBJECT(mainWindow),"delete-event",G_CALLBACK(doShutdown),NULL);
	accgroup=gtk_accel_group_new();
	gtk_window_add_accel_group((GtkWindow*)mainWindow,accgroup);

	setUpContextMenus();

	mainNotebook=(GtkNotebook*)gtk_notebook_new();
	gtk_notebook_set_scrollable(mainNotebook,true);
	g_signal_connect(G_OBJECT(mainNotebook),"switch-page",G_CALLBACK(switchPage),NULL);

	mainVBox=createNewBox(NEWVBOX,false,0);

//main menus
//add menubar
	buildMenus();
	gtk_box_pack_start((GtkBox*)mainVBox,menuBar,false,false,0);

//add toolbar
	toolBar=(GtkToolbar*)gtk_toolbar_new();
	gtk_toolbar_set_style((GtkToolbar*)toolBar,GTK_TOOLBAR_BOTH);
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

	diskList=gtk_list_store_new(NUMDISKCOLS,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_BOOLEAN);
	diskView=(GtkTreeView*)gtk_tree_view_new_with_model((GtkTreeModel*)diskList);
	gtk_tree_view_set_headers_visible(diskView,false);
	g_signal_connect(diskView,"row-activated",G_CALLBACK(openDisk),NULL);	
	g_signal_connect(diskView,"button-press-event",G_CALLBACK(buttonDownDisk),NULL);	
//pix buf
	renderer=gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_insert_column_with_attributes((GtkTreeView*)diskView,-1,"icon",renderer,"pixbuf",DEVPIXBUF,NULL);
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

	bmList=gtk_list_store_new(NUMBMS,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_STRING);
	bmView=(GtkTreeView*)gtk_tree_view_new_with_model((GtkTreeModel*)bmList);
	gtk_tree_view_set_headers_visible(bmView,false);
	gtk_tree_view_set_reorderable(bmView,true);

	g_signal_connect(bmView,"row-activated",G_CALLBACK(openBM),NULL);	
	g_signal_connect(bmView,"button-press-event",G_CALLBACK(buttonDownBM),NULL);	
//bm pix buf
	renderer=gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_insert_column_with_attributes((GtkTreeView*)bmView,-1,"icon",renderer,"pixbuf",BMPIXBUF,NULL);
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
	statusBar=gtk_statusbar_new();
	statusID=gtk_statusbar_get_context_id((GtkStatusbar*)statusBar,"stuff");
	gtk_box_pack_start((GtkBox*)mainVBox,statusBar,false,false,0);

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

void loadPixbufs(void)
{
	GtkIconInfo	*info=NULL;
	GIcon		*icon=NULL;
	int			iconsize=24;

	icon=g_content_type_get_icon("text-x-generic");
	info=gtk_icon_theme_lookup_by_gicon(defaultTheme,icon,iconSize,(GtkIconLookupFlags)0);
	if(info==NULL)
		info=gtk_icon_theme_lookup_by_gicon(gnomeTheme,icon,iconSize,(GtkIconLookupFlags)0);

	if(genericText!=NULL)
		{
			if(G_IS_OBJECT(genericText))
				g_object_unref(genericText);
		}
	
	genericText=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,iconSize,NULL);
	if(G_IS_OBJECT(icon))
		g_object_unref(icon);
#ifdef _USEGTK3_
	if(G_IS_OBJECT(info))
		g_object_unref(info);
#else
	if(info!=NULL)
		gtk_icon_info_free(info);
#endif

	for(int j=0;j<NUMPBS;j++)
		{
			if(guiPixbufs[j]!=NULL)
				{
					g_object_unref(guiPixbufs[j]);
					guiPixbufs[j]=NULL;
				}
			info=gtk_icon_theme_lookup_icon(defaultTheme,iconNames[j],iconsize,(GtkIconLookupFlags)0);
			if(info==NULL)
				info=gtk_icon_theme_lookup_icon(gnomeTheme,iconNames[j],iconsize,(GtkIconLookupFlags)0);			
			if(info==NULL)
				guiPixbufs[j]=NULL;
			else
				guiPixbufs[j]=gdk_pixbuf_new_from_file_at_size(gtk_icon_info_get_filename(info),-1,iconsize,NULL);
#ifdef _USEGTK3_
			if(G_IS_OBJECT(info))
				g_object_unref(info);
#else
			if(info!=NULL)
				gtk_icon_info_free(info);
#endif
		}

	if(guiPixbufs[USBDISKPB]==NULL)
		{
			guiPixbufs[USBDISKPB]=guiPixbufs[HDDRIVEPB];
			g_object_ref(guiPixbufs[USBDISKPB]);
		}
	if(guiPixbufs[ROMPB]==NULL)
		{
			guiPixbufs[ROMPB]=guiPixbufs[HDDRIVEPB];
			g_object_ref(guiPixbufs[ROMPB]);
		}
	if(guiPixbufs[DVDROMPB]==NULL)
		{
			guiPixbufs[DVDROMPB]=guiPixbufs[ROMPB];
			g_object_ref(guiPixbufs[DVDROMPB]);
		}
	if(guiPixbufs[CDROMPB]==NULL)
		{
			guiPixbufs[CDROMPB]=guiPixbufs[DVDROMPB];
			g_object_ref(guiPixbufs[CDROMPB]);
		}
}

void setStatusMessage(const char *msg)
{
	gtk_statusbar_remove_all((GtkStatusbar*)statusBar,statusID);
	gtk_statusbar_push ((GtkStatusbar*)statusBar,statusID,msg);
}

void buildMessgage(pageStruct *pagex)
{
	int					cnt;
	char				**array=NULL;
	struct stat			st;
	int64_t				totsize=0;
	const char			*file=NULL;
	int					charsprinted;
	bool				ignore=false;
	char				*command=(char*)alloca(PATH_MAX);
	char				*buffer=(char*)alloca(PATH_MAX);
	pageStruct			*page=getPageFromCurrentTab();
	humanUnitsStruct	hu;

	if(page==NULL)
		return;

	cnt=selectionToArray(&array,false);
	if(cnt!=0)
		{
			totsize=0;
			if(cnt==1)
				file=array[0];

			for(int j=0;j<cnt;j++)
				{
					stat(array[j],&st);
					totsize+=st.st_size;
				}

			bytesToHuman(totsize,&hu);
			if(cnt>1)
				{
					if(page->buttonDown==true)
						ignore=true;
					charsprinted=sprintf(buffer,"Total size of %i files=%.02f%c",cnt,hu.number,hu.suffix);
				}
			else
				charsprinted=sprintf(buffer,"Size of %s=%.02f%c",file,hu.number,hu.suffix);

			if(page->searchString!=NULL && strlen(page->searchString)>0)
				sprintf(&buffer[charsprinted],", Search=\"%s\"",page->searchString);
		}
	else
		{
			sprintf(command,"df -h \"%s\"|tail -n1|awk '{print \"Disk \" $1 \": Size \" $2 \" Used \" $3 \" Avail \" $4 \" Usage \" $5 \" Mountpoint \" $6}'",page->thisFolder);
			sinkReturnStr=oneLiner(command,buffer);
		}

	if(ignore==false)
		setStatusMessage(buffer);
	g_strfreev(array);
}






