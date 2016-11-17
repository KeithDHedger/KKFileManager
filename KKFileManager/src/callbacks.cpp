/*
 *
 * ©K. D. Hedger. Thu 17 Nov 13:06:08 GMT 2016 kdhedger68713@gmail.com

 * This file (callbacks.cpp) is part of KKFileManager.

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
 
#include <gtk/gtk.h>

#include "gui.h"

void dirChanged(GFileMonitor *monitor,GFile *file,GFile *other_file,GFileMonitorEvent event_type,pageStruct *page)
{
	if((G_FILE_MONITOR_EVENT_CHANGED==event_type) || (G_FILE_MONITOR_EVENT_DELETED==event_type) || (G_FILE_MONITOR_EVENT_CREATED==event_type) || (G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED==event_type))
		populatePageStore(page);
}

