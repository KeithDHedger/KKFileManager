#if 0

#©keithhedger Fri 14 Oct 12:30:59 BST 2016 kdhedger68713@gmail.com

#g++ -Wall -D _USEGTK3_ "$0" properties.cpp $(pkg-config --cflags --libs gtk+-3.0)||exit 1
g++ -Wall "$0" properties.cpp $(pkg-config --cflags --libs gtk+-2.0)||exit 1

./a.out "$@"
retval=0
rm ./a.out
exit 

#endif

#include <stdio.h>
#include "properties.h"

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);
	doFileProps(NULL,NULL);
#ifndef _FileProps_ISDIALOG_
    	gtk_main();
#endif

	return 0;
}
