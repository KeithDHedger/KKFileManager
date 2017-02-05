/*
 *
 * ©K. D. Hedger. Sun  5 Feb 18:28:05 GMT 2017 kdhedger68713@gmail.com

 * This file (mimetype.h) is part of KKFileManager.

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

#ifndef _MIMETYPE_
#define _MIMETYPE_

void mime_type_init(void);
char *getFIcon(const char *mime_type,char *name);
const char* mime_type_get_by_file(const char* filepath,struct stat* statbuf,const char* basename);
char *getDIcon(const char *mime_type,char *name);

#endif
