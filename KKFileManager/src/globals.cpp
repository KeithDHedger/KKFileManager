/*
 *
 * ©K. D. Hedger. Sat 12 Nov 15:13:28 GMT 2016 kdhedger68713@gmail.com

 * This file (globals.cpp) is part of KKFileManager.

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
#include <magic.h>
#include <map>

#include "globals.h"

//main app
GtkToolbar		*toolBar=NULL;
GtkWidget		*toolBarBox=NULL;
GtkWidget		*mainWindow = NULL;
GtkWidget		*mainVBox=NULL;
char			*sinkReturnStr=NULL;
GtkIconTheme	*defaultTheme=NULL;
GtkIconTheme	*gnomeTheme=NULL;
GtkNotebook		*mainNotebook=NULL;
GList			*pageList=NULL;
GtkAccelGroup	*accgroup=NULL;
GtkWidget		*mainHPane=NULL;
GtkWidget		*leftVBox=NULL;
GtkWidget		*leftVPane=NULL;
GtkListStore	*diskList=NULL;
GtkTreeView		*diskView=NULL;
GtkListStore	*bmList=NULL;
GtkTreeView		*bmView=NULL;
GtkWidget		*menuBar=NULL;
int				sessionID=-1;
GApplication	*mainApp;
bool			openDefault=false;
GtkClipboard	*mainClipboard=NULL;
GValue			boolVal=G_VALUE_INIT;

//file menu
GtkWidget		*fileMenu=NULL;
GtkWidget		*menuItemNew=NULL;
GtkWidget		*menuItemTerminal=NULL;
GtkWidget		*menuItemPrefs=NULL;

//tools menu
GtkWidget		*toolsMenu=NULL;
GtkWidget		*toolsSubMenu=NULL;
GList			*toolsList=NULL;
int				intermarg=0;
int				flagsarg=0;
int				inpopup=0;
int				inmenu=0;
int				inboth=0;
char			*commandarg=NULL;
char			*menuname=NULL;

args			tool_vars[]=
{
	//strings
	{"name",TYPESTRING,&menuname},
	{"command",TYPESTRING,&commandarg},
	//ints
	{"interm",TYPEINT,&intermarg},
	{"flags",TYPEINT,&flagsarg},
	{"inpopup",TYPEBOOL,&inpopup},
	{"inmenu",TYPEBOOL,&inmenu},
	{"inboth",TYPEBOOL,&inboth},
	{NULL,0,NULL}
};

//help menu
GtkWidget		*helpMenu=NULL;
GtkWidget		*aboutMenu=NULL;

//tool bar
GtkToolItem		*refreshButton=NULL;
GtkToolItem		*upButton=NULL;
GtkToolItem		*backButton=NULL;
GtkToolItem		*forwardButton=NULL;
GtkToolItem		*homeButton=NULL;
GtkToolItem		*newButton=NULL;
GtkToolItem		*locationButton=NULL;
GtkEntry		*locationTextBox=NULL;

//mimes and stuff
std::map<unsigned,GdkPixbuf*> pixBuffCache;
GFile			*dirPath=NULL;
GFileMonitor	*monitorDir=NULL;

//tabs
unsigned		pageCnt=1000;
unsigned		iconPadding=16;
unsigned		iconSize=ICONSIZE;
unsigned		iconSize3=ICONSIZE;
unsigned		maxTabChars=32;

//menus

//prefs
bool			showMenuIcons=true;
char			*toolBarLayout=NULL;
char			*diskIncludePattern=NULL;
char			*diskExcludePattern=NULL;
char			*terminalCommand=NULL;
bool			showHidden=false;

//save and load var lists
char			*windowAllocData=NULL;
int				windowX=-1;
int				windowY=-1;
int				windowWidth=1000;
int				windowHeight=600;
int				leftPaneWidth=260;
int				leftPaneHeight=400;

args			kkfilemanager_rc[]=
{
	//bools
	//strings
	{"toolbarlayout",TYPESTRING,&toolBarLayout},
	{"includedisks",TYPESTRING,&diskIncludePattern},
	{"excludedisks",TYPESTRING,&diskExcludePattern},
	{"terminalcommand",TYPESTRING,&terminalCommand},
	{"windowsize",TYPESTRING,&windowAllocData},
	//ints
	{"leftpanewidth",TYPEINT,&leftPaneWidth},
	{"leftpanehite",TYPEINT,&leftPaneHeight},
	//lists
	{NULL,0,NULL}
};

//tests
//GdkPixbuf		*testpb=NULL;

//pixmaps
GdkPixbuf		*genericText=NULL;
GdkPixbuf		*diskPB=NULL;
GdkPixbuf		*bookmarkPB=NULL;
GdkPixbuf		*homePB=NULL;
GdkPixbuf		*computerPB=NULL;
GdkPixbuf		*symLink=NULL;
GdkPixbuf		*brokenLink=NULL;

GdkPixbuf		*guiPixbufs[NUMPBS]={NULL,};

//dand
unsigned		fromPageID=0;

//odds
int				sinkReturn;

//global functions
char* oneLiner(const char *command)
{
	FILE	*fp=NULL;
	char	*retstr=(char*)calloc(1,256);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			sinkReturnStr=fgets(retstr,256,fp);
			pclose(fp);
		}
	if(sinkReturnStr==NULL)
		{
			free(retstr);
			return(NULL);
		}
	else
		{
			if(retstr[strlen(retstr)-1]=='\n')
				retstr[strlen(retstr)-1]=0;
		}
	return(retstr);
}

void setCurrentFolderForTab(const char *newfolder,pageStruct *page,bool updatenav,bool force)
{
	GtkEntryCompletion	*completion;
	GtkListStore		*list;
	char				*holdfolder=NULL;

	if(g_file_test(newfolder,G_FILE_TEST_IS_DIR)==false)
		return;

	if((force==false) && (strcmp(page->thisFolder,newfolder)==0))
		return;

	if(newfolder!=NULL && strlen(newfolder)>0)
		{
			if(updatenav==true)
				addToHistory(page,page->thisFolder,true);
			holdfolder=strdup(newfolder);
			free(page->thisFolder);
			page->thisFolder=holdfolder;
			gtk_entry_set_text(locationTextBox,page->thisFolder);
			gtk_editable_set_position((GtkEditable*)locationTextBox,-1);
			populatePageStore(page);
			monitorFolderForPage(page);
			completion=gtk_entry_get_completion(locationTextBox);
			list=(GtkListStore*)gtk_entry_completion_get_model(completion);
        	gtk_list_store_clear(list);
			gtk_entry_completion_complete(completion);
			gtk_widget_show_all(page->vBox);
			updateTabLabel(page);
		}
}

int yesNo(const char *question,char *file)
{
	GtkWidget	*dialog;
	int			result;

	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_YES_NO,"%s %s",question,file);
	gtk_window_set_title(GTK_WINDOW(dialog),"What Do You Want To Do?");
	result=gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	return(result);
}

void information(const char *info,char *file)
{
	GtkWidget	*dialog;

	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"%s %s",info,file);
	sinkReturn=gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

varStrings* allocVStrings(char *string)
{
	int	namelen=0;
	int	datalen=0;
	int	totalcnt=0;

	varStrings	*vs=(varStrings*)calloc(1,sizeof(varStrings));
	vs->name=NULL;
	vs->data=NULL;

//var name
	sscanf(string,"%*s%n",&namelen);
	vs->name=strndup(string,namelen);
	totalcnt=namelen;

//var type
	sscanf(string,"%*s %n%*[^\n]s",&totalcnt);
	datalen=(int)(long)((long)strchr(string,'\n')-(long)string)-totalcnt;
	vs->data=strndup(&string[totalcnt],datalen);

	return(vs);
}

int loadVarsFromFile(char *filepath,args *dataptr)
{
	FILE		*fd=NULL;
	char		buffer[2048];
	int			cnt;
	int			retval=NOERR;
	varStrings	*vs=NULL;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					sinkReturnStr=fgets(buffer,2048,fd);
					vs=allocVStrings(buffer);
					cnt=0;
					while(dataptr[cnt].name!=NULL)
						{
							if((vs->data!=NULL) &&(vs->name!=NULL) &&(strcmp(vs->name,dataptr[cnt].name)==0))
								{
									switch(dataptr[cnt].type)
										{
											case TYPEINT:
												*(int*)dataptr[cnt].data=atoi(vs->data);
												break;
											case TYPESTRING:
												if(*(char**)(dataptr[cnt].data)!=NULL)
													{
														free(*(char**)(dataptr[cnt].data));
													}
												*(char**)(dataptr[cnt].data)=(char*)strdup(vs->data);
												break;
											case TYPEBOOL:
												*(bool*)dataptr[cnt].data=(bool)atoi(vs->data);
												break;
											case TYPELIST:
												*(GSList**)dataptr[cnt].data=g_slist_append(*(GSList**)dataptr[cnt].data,strdup(vs->data));
												break;
										}
								}
							cnt++;
						}
					free(vs->name);
					free(vs->data);
					free(vs);
				}
			fclose(fd);
		}
	else
		{
			retval=NOOPENFILE;
		}

	return(retval);
}

void saveVarsToFile(char *filepath,args *dataptr)
{
	FILE	*fd=NULL;
	int		cnt=0;
//	GSList	*list=NULL;

	fd=fopen(filepath,"w");
	if(fd!=NULL)
		{
			while(dataptr[cnt].name!=NULL)
				{
					switch(dataptr[cnt].type)
						{
							case TYPEINT:
								fprintf(fd,"%s	%i\n",dataptr[cnt].name,*(int*)dataptr[cnt].data);
								break;
							case TYPESTRING:
								fprintf(fd,"%s	%s\n",dataptr[cnt].name,*(char**)(dataptr[cnt].data));
								break;
							case TYPEBOOL:
								fprintf(fd,"%s	%i\n",dataptr[cnt].name,(int)*(bool*)dataptr[cnt].data);
								break;
							case TYPELIST:
//								list=*(GSList**)((dataptr[cnt].data));
//								if(g_slist_length(list)>maxFRHistory)
//									list=g_slist_nth(list,g_slist_length(list)-maxFRHistory);
//								while(list!=NULL)
//									{
//										if(strlen((char*)list->data)>0)
//											{
//												fprintf(fd,"%s %s\n",dataptr[cnt].name,(char*)list->data);
//											}
//										list=list->next;
//									}
								break;
						}
					cnt++;
				}
			fclose(fd);
		}
}

void writeExitData(void)
{
	GtkAllocation	alloc;
	char			*filename;
	int				winx;
	int				winy;
	GtkTreeIter		iter;
	char			*path;
	bool			validiter=false;
	FILE			*fp=NULL;
	char			*label=NULL;

	gtk_widget_get_allocation(mainWindow,&alloc);
	gtk_window_get_position((GtkWindow*)mainWindow,&winx,&winy);
	if( (alloc.width>10) && (alloc.height>10) )
		sinkReturn=asprintf(&windowAllocData,"%i %i %i %i",alloc.width,alloc.height,winx,winy);

	leftPaneWidth=gtk_paned_get_position((GtkPaned*)mainHPane);
	leftPaneHeight=gtk_paned_get_position((GtkPaned*)leftVPane);

	sinkReturn=asprintf(&filename,"%s/%s",getenv("HOME"),APPFOLDENAME);
	g_mkdir_with_parents(filename,493);
	free(filename);
	sinkReturn=asprintf(&filename,"%s/%s/kkfiemanager.rc",getenv("HOME"),APPFOLDENAME);

	saveVarsToFile(filename,kkfilemanager_rc);

	free(filename);
	free(windowAllocData);

	validiter=gtk_tree_model_get_iter_first(GTK_TREE_MODEL(bmList),&iter);
	asprintf(&filename,"%s/.KKFileManager/bookmarks",getenv("HOME"));
	fp=fopen(filename,"w");
	if(fp!=NULL)
		{
			while(validiter==true)
				{
					gtk_tree_model_get(GTK_TREE_MODEL(bmList),&iter,BMPATH,&path,BMLABEL,&label,-1);
					if(strcmp(label,"Home")==0)
						{
							free(label);
							label=NULL;
						}
					else if(strcmp(label,"Computer")==0)
						{
							free(label);
							label=NULL;
						}
					else if(strcmp(label,"Desktop")==0)
						{
							free(label);
							label=NULL;
						}
					if(label!=NULL)
						{
							fputs(path,fp);
							fputc('\n',fp);
							free(path);
							free(label);
						}
					validiter=gtk_tree_model_iter_next(GTK_TREE_MODEL(bmList),&iter);
				}
			fclose(fp);
		}
	free(filename);
}

char *selectionToString(const char *seperator)
{
	pageStruct		*page=NULL;
	char			*path;
	GtkTreeIter		iter;
	GList			*iconlist;
	GString			*str=NULL;

	page=getPageStructByIDFromList(getPageIdFromTab());
	if(page!=NULL)
		{
			iconlist=gtk_icon_view_get_selected_items(page->iconView);
			if(iconlist!=NULL)
				{
					str=g_string_new(NULL);
					while(iconlist!=NULL)
						{
							gtk_tree_model_get_iter(GTK_TREE_MODEL(page->listStore),&iter,(GtkTreePath*)iconlist->data);
							gtk_tree_model_get(GTK_TREE_MODEL(page->listStore),&iter,FILEPATH,&path,-1);
							if(path!=NULL)
								{
									g_string_append_printf(str,"%s%s",path,seperator);
									free(path);
								}
							iconlist=iconlist->next;
						}
					g_list_foreach(iconlist,(GFunc)gtk_tree_path_free,NULL);
					g_list_free(iconlist);
					return(g_string_free(str,false));
				}
		}
	return(NULL);
}

unsigned selectionToArray(char ***array,bool touri)
{
	pageStruct		*page=NULL;
	char			*path;
	GtkTreeIter		iter;
	GList			*iconlist;
	char			**ar=NULL;
	unsigned		cnt=0;
	gchar			*filename=NULL;
	unsigned		arraylen=0;

	page=getPageStructByIDFromList(getPageIdFromTab());
	if(page!=NULL)
		{
			iconlist=gtk_icon_view_get_selected_items(page->iconView);
			arraylen=g_list_length(iconlist);
			if(iconlist!=NULL)
				{
					*array=(char**)calloc(g_list_length(iconlist)+1,sizeof(char*));
					ar=*array;
					while(iconlist!=NULL)
						{
							gtk_tree_model_get_iter(GTK_TREE_MODEL(page->listStore),&iter,(GtkTreePath*)iconlist->data);
							gtk_tree_model_get(GTK_TREE_MODEL(page->listStore),&iter,FILEPATH,&path,-1);
							if(path!=NULL)
								{
									if(touri==true)
										{
											filename=g_filename_to_uri(path,NULL,NULL);
											if(filename!=NULL)
												{
													ar[cnt]=strdup(filename);
													free(filename);
												}
										}
									else
										{
											ar[cnt]=strdup(path);
										}
									cnt++;
									free(path);
								}
							iconlist=iconlist->next;
						}
					g_list_foreach(iconlist,(GFunc)gtk_tree_path_free,NULL);
					g_list_free(iconlist);
					ar[cnt]=NULL;
				}
		}
	return(arraylen);
}

bool checkAccess(const char *path)
{
	int mode;

	if(g_file_test(path,G_FILE_TEST_IS_DIR))
		{
			mode=R_OK|X_OK;
		}
	else
		mode=F_OK;
	mode=access(path,mode);
	return(!mode);
}




