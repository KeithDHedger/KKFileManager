/*
 *
 * ©K. D. Hedger. Thu 17 Nov 13:06:14 GMT 2016 kdhedger68713@gmail.com

 * This file (callbacks.h) is part of KKFileManager.

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


#ifndef _CALLBACKS_
#define _CALLBACKS_

void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct *page);

#endif
