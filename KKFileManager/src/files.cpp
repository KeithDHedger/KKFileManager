/*
 *
 * ©K. D. Hedger. Mon 23 Jan 10:56:19 GMT 2017 kdhedger68713@gmail.com

 * This file (files.cpp) is part of KKFileManager.

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

//#include <stdlib.h>
//#include <string.h>
//#include <sys/stat.h>
//#include <gtk/gtk.h>
//#include <gdk/gdk.h>
//#include <gdk/gdkkeysyms.h>

#include "globals.h"

void getValidToPathFromFilepath(filePathStruct *fps)
{
	char	*tmppathbuffer=(char*)alloca(PATH_MAX);
	char	*tname;
	char	*tdir;

	tname=g_path_get_basename(fps->filePath);
	tdir=g_path_get_dirname(fps->filePath);
	getUniqueFilenameOnly(fps);

	if(fps->askFileName==true)
		{
			doAskForFilename(fps->fileName);
			if(validName==true)
				{
					snprintf(tmppathbuffer,PATH_MAX,"%s/%s",fps->toDirPath,fileName);
					setFilePathStruct(fps,"f",tmppathbuffer);
				}
			else
				{
					snprintf(tmppathbuffer,PATH_MAX,"%s/%s",tdir,tname);
					setFilePathStruct(fps,"f",tmppathbuffer);
					fps->modified=false;
				}
		}
	else
		{
			snprintf(tmppathbuffer,PATH_MAX,"%s/%s",fps->toDirPath,fps->fileName);
			setFilePathStruct(fps,"f",tmppathbuffer);
		}

	free(tname);
	free(tdir);
}

/*
format:
	d=set from dirpath
	n=set filename
	f=set filepath
	t=set to dirpath
	p=set dirname and filename from filepath
	P=set filepath from dirname and filename
	T=set filepath from to dirpath and filename
*/
void setFilePathStruct(filePathStruct *fps,const char *format,...)
{
	va_list	ap;
	char	*teststr=NULL;
	bool	modified=false;

	va_start(ap,format);
	while(*format)
		{
			switch(*format++)
				{
					case 'd':
						modified=true;
						if(fps->dirPath!=NULL)
							free(fps->dirPath);
						fps->dirPath=NULL;
						teststr=va_arg(ap,char*);
						if(teststr!=NULL)
							fps->dirPath=strdup(teststr);
						break;
					case 'n':
						modified=true;
						if(fps->fileName!=NULL)
							free(fps->fileName);
						fps->fileName=NULL;
						teststr=va_arg(ap,char*);
						if(teststr!=NULL)
							fps->fileName=strdup(teststr);
						break;
					case 'f':
						modified=true;
						if(fps->filePath!=NULL)
							free(fps->filePath);
						fps->filePath=NULL;
						teststr=va_arg(ap,char*);
						if(teststr!=NULL)
							fps->filePath=strdup(teststr);
						break;
					case 't':
						modified=true;
						if(fps->toDirPath!=NULL)
							free(fps->toDirPath);
						fps->toDirPath=NULL;
						teststr=va_arg(ap,char*);
						if(teststr!=NULL)
							fps->toDirPath=strdup(teststr);
						break;
					case 'p':
						modified=true;
						if(fps->dirPath!=NULL)
							free(fps->dirPath);
						if(fps->fileName!=NULL)
							free(fps->fileName);
						fps->dirPath=g_path_get_dirname(fps->filePath);
						fps->fileName=g_path_get_basename(fps->filePath);
						break;
					case 'P':
						modified=true;
						if(fps->filePath!=NULL)
							free(fps->filePath);
						asprintf(&fps->filePath,"%s/%s",fps->dirPath,fps->fileName);
						break;
					case 'T':
						modified=true;
						if(fps->filePath!=NULL)
							free(fps->filePath);
						asprintf(&fps->filePath,"%s/%s",fps->toDirPath,fps->fileName);
						break;
				}
		}
	va_end(ap);
	fps->modified=modified;
	if(fps->filePath!=NULL)
		fps->filePathIsDir=g_file_test(fps->filePath,G_FILE_TEST_IS_DIR);
}

void getUniqueFilenameOnly(filePathStruct *fps)
{
	char		buffer[PATH_MAX+20];
	unsigned	cnt=0;
	char		*namebuffer=(char*)alloca(NAME_MAX+1);
	bool		modded=false;

	namebuffer=g_path_get_basename(fps->filePath);
	snprintf(buffer,PATH_MAX,"%s/%s",fps->toDirPath,namebuffer);
	cnt=1;

	if(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
		{
			while(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
				snprintf(buffer,PATH_MAX,"%s/%s %u",fps->toDirPath,namebuffer,cnt++);
			cnt--;
			free(fps->fileName);
			asprintf(&fps->fileName,"%s %u",namebuffer,cnt);
			modded=true;
		}
	else
		{
			free(fps->fileName);
			asprintf(&fps->fileName,"%s",namebuffer);
		}

	fps->modified=modded;
}

filePathStruct* getValidFilepath(const char *filepath)
{
	char			buffer[PATH_MAX];
	unsigned		cnt;
	filePathStruct	*fs=(filePathStruct*)calloc(1,sizeof(filePathStruct));
	char			*filename=g_path_get_basename(filepath);

	fs->dirPath=g_path_get_dirname(filepath);
	fs->modified=false;

	sprintf(buffer,"%s/%s",fs->dirPath,filename);

	if(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
		{
			cnt=0;
			while(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
				{
					cnt++;
					sprintf(buffer,"%s/%s-%u",fs->dirPath,filename,cnt);
				}
			fs->modified=true;
		}
	else
		sprintf(buffer,"%s",filepath);

	fs->filePath=strdup(buffer);
	fs->fileName=g_path_get_basename(buffer);
	free(filename);
	return(fs);
}


void freefilePathStruct(filePathStruct* fs)
{
	if(fs->dirPath!=NULL)
		free(fs->dirPath);
	if(fs->fileName!=NULL)
		free(fs->fileName);
	if(fs->filePath!=NULL)
		free(fs->filePath);
	if(fs->toDirPath!=NULL)
		free(fs->toDirPath);
	free(fs);
}

