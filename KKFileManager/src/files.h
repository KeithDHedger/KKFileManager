/*
 *
 * ©K. D. Hedger. Mon 23 Jan 10:56:12 GMT 2017 keithdhedger@gmail.com

 * This file (files.h) is part of KKFileManager.

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


#ifndef _FILES_
#define _FILES_

#include "globals.h"

#define MIME_OPEN_FILE 10000

struct	filePathStruct
{
	char	*fromDirPath;
	char	*fromFileName;
	char	*fromFilePath;

	char	*toDirPath;
	char	*toFileName;
	char	*toFilePath;

	bool	modified;
	bool	askFileName;
	bool	fromFilePathIsDir;
	bool	toFilePathIsDir;
};

void printfilestruct(filePathStruct *fps);

void getValidToPathFromFilepath(filePathStruct *fps);
void getUniqueFilenameOnly(filePathStruct *fps);
void setFilePathStruct(filePathStruct *fps,const char *format,...);
void freefilePathStruct(filePathStruct* fs,bool freestruct);
filePathStruct* getValidFilepath(const char *filepath);
void doFileAction(filePathStruct *fps,int action);
void printfFileStruct(filePathStruct *fps);

#endif
