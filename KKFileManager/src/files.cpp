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

	tname=g_path_get_basename(fps->fromFilePath);
	tdir=g_path_get_dirname(fps->fromFilePath);
	getUniqueFilenameOnly(fps);

	if(fps->askFileName==true)
		{
			doAskForFilename(fps->toFileName);
			if(validName==true)
				{
					snprintf(tmppathbuffer,PATH_MAX,"%s/%s",fps->toDirPath,fileName);
					setFilePathStruct(fps,"FN",tmppathbuffer,fileName);
				}
			else
				{
					setFilePathStruct(fps,"FND",NULL,NULL,NULL);
					fps->modified=false;
				}
		}
	else
		{
			snprintf(tmppathbuffer,PATH_MAX,"%s/%s",fps->toDirPath,fps->fromFileName);
			setFilePathStruct(fps,"F",tmppathbuffer);
		}

	free(tname);
	free(tdir);
}

/*
format:
	d=set from dirpath
	n=set filename
	f=set from filepath

	D=set to dirpath
	N=set to filename
	F=set to filepath

	p=set dirname and filename from filepath
	P=set filepath from dirname and filename

	t=set to dirname and to filename from to filepath
	T=set to filepath from to dirpath and filename

	c=set to filepath from from filename and to dirpath
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
						if(fps->fromDirPath!=NULL)
							free(fps->fromDirPath);
						fps->fromDirPath=NULL;
						teststr=va_arg(ap,char*);
						if(teststr!=NULL)
							fps->fromDirPath=strdup(teststr);
						break;
					case 'D':
						modified=true;
						if(fps->toDirPath!=NULL)
							free(fps->toDirPath);
						fps->toDirPath=NULL;
						teststr=va_arg(ap,char*);
						if(teststr!=NULL)
							fps->toDirPath=strdup(teststr);
						break;
					case 'n':
						modified=true;
						if(fps->fromFileName!=NULL)
							free(fps->fromFileName);
						fps->fromFileName=NULL;
						teststr=va_arg(ap,char*);
						if(teststr!=NULL)
							fps->fromFileName=strdup(teststr);
						break;
					case 'N':
						modified=true;
						if(fps->toFileName!=NULL)
							free(fps->toFileName);
						fps->toFileName=NULL;
						teststr=va_arg(ap,char*);
						if(teststr!=NULL)
							fps->toFileName=strdup(teststr);
						break;
					case 'f':
						modified=true;
						if(fps->fromFilePath!=NULL)
							free(fps->fromFilePath);
						fps->fromFilePath=NULL;
						teststr=va_arg(ap,char*);
						if(teststr!=NULL)
							fps->fromFilePath=strdup(teststr);
						break;
					case 'F':
						modified=true;
						if(fps->toFilePath!=NULL)
							free(fps->toFilePath);
						fps->toFilePath=NULL;
						teststr=va_arg(ap,char*);
						if(teststr!=NULL)
							fps->toFilePath=strdup(teststr);
						break;
					case 'p':
						modified=true;
						if(fps->fromDirPath!=NULL)
							free(fps->fromDirPath);
						if(fps->fromFileName!=NULL)
							free(fps->fromFileName);
						fps->fromDirPath=g_path_get_dirname(fps->fromFilePath);
						fps->fromFileName=g_path_get_basename(fps->fromFilePath);
						break;
					case 'P':
						modified=true;
						if(fps->fromFilePath!=NULL)
							free(fps->fromFilePath);
						asprintf(&fps->fromFilePath,"%s/%s",fps->fromDirPath,fps->fromFileName);
						break;
					case 't':
						modified=true;
						if(fps->toDirPath!=NULL)
							free(fps->toDirPath);
						if(fps->toFileName!=NULL)
							free(fps->toFileName);
						fps->toDirPath=g_path_get_dirname(fps->toFilePath);
						fps->toFileName=g_path_get_basename(fps->toFilePath);
						break;
					case 'T':
						modified=true;
						if(fps->toFilePath!=NULL)
							free(fps->toFilePath);
						asprintf(&fps->toFilePath,"%s/%s",fps->toDirPath,fps->toFileName);
						break;
					case 'c':
						modified=true;
						if(fps->toFilePath!=NULL)
							free(fps->toFilePath);
						asprintf(&fps->toFilePath,"%s/%s",fps->toDirPath,fps->fromFileName);
						break;
						
				}
		}
	va_end(ap);
	fps->modified=modified;
	if(fps->fromFilePath!=NULL)
		fps->fromFilePathIsDir=g_file_test(fps->fromFilePath,G_FILE_TEST_IS_DIR);
	if(fps->toFilePath!=NULL)
		fps->toFilePathIsDir=g_file_test(fps->toFilePath,G_FILE_TEST_IS_DIR);
}

void getUniqueFilenameOnly(filePathStruct *fps)
{
	char		buffer[PATH_MAX+20];
	unsigned	cnt=0;
	char		*namebuffer=(char*)alloca(NAME_MAX+1);
	bool		modded=false;

	namebuffer=g_path_get_basename(fps->fromFilePath);
	snprintf(buffer,PATH_MAX,"%s/%s",fps->toDirPath,namebuffer);
	cnt=1;

	if(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
		{
			while(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
				snprintf(buffer,PATH_MAX,"%s/%s %u",fps->toDirPath,namebuffer,cnt++);
			cnt--;
			free(fps->toFileName);
			asprintf(&fps->toFileName,"%s %u",namebuffer,cnt);
			modded=true;
		}
	else
		{
			free(fps->toFileName);
			asprintf(&fps->toFileName,"%s",namebuffer);
		}

	fps->modified=modded;
}

filePathStruct* getValidFilepath(const char *filepath)
{
	char			buffer[PATH_MAX];
	unsigned		cnt;
	filePathStruct	*fs=(filePathStruct*)calloc(1,sizeof(filePathStruct));
	char			*filename=g_path_get_basename(filepath);

	fs->fromDirPath=g_path_get_dirname(filepath);
	fs->modified=false;

	sprintf(buffer,"%s/%s",fs->fromDirPath,filename);

	if(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
		{
			cnt=0;
			while(g_file_test(buffer,G_FILE_TEST_EXISTS)==true)
				{
					cnt++;
					sprintf(buffer,"%s/%s-%u",fs->fromDirPath,filename,cnt);
				}
			fs->modified=true;
		}
	else
		sprintf(buffer,"%s",filepath);

	fs->fromFilePath=strdup(buffer);
	fs->fromFileName=g_path_get_basename(buffer);
	free(filename);
	return(fs);
}


void freefilePathStruct(filePathStruct* fs)
{
	if(fs->fromDirPath!=NULL)
		free(fs->fromDirPath);
	if(fs->fromFileName!=NULL)
		free(fs->fromFileName);
	if(fs->fromFilePath!=NULL)
		free(fs->fromFilePath);
	if(fs->toDirPath!=NULL)
		free(fs->toDirPath);
	free(fs);
}

void doFileAction(filePathStruct *fps,int action)
{
	const char	*command;
	char		*buffer;

	switch(action)
		{
			case GDK_ACTION_COPY:
				if(fps->fromFilePathIsDir==true)
					command="cp -r";
				else
					command="cp";
				break;
			case GDK_ACTION_MOVE:
				command="mv";
				break;
			case GDK_ACTION_LINK:
				command="ln -s";
				break;
			default:
				printf("error\n");
				return;
		}

	asprintf(&buffer,"%s \"%s\" \"%s\"",command,fps->fromFilePath,fps->toFilePath);
	system(buffer);
	free(buffer);
}

void printfilestruct(filePathStruct *fps)
{
printf("dirPath=%s\n",fps->fromDirPath);
printf("fileName=%s\n",fps->fromFileName);
printf("filePath=%s\n",fps->fromFilePath);

printf("toDirPath=%s\n",fps->toDirPath);
printf("toFileName=%s\n",fps->toFileName);
printf("toFilePath=%s\n",fps->toFilePath);

printf("modified=%i\n",fps->modified);
printf("askFileName=%i\n",fps->askFileName);
printf("fromFilePathIsDir=%i\n",fps->fromFilePathIsDir);
printf("toFilePathIsDir=%i\n",fps->toFilePathIsDir);
}






