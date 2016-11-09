

include ../flagsandlibs
include ../sources

kkfilemanagerdebug_SOURCES = $(COMMONSRC)

kkfilemanagerdebug_CFLAGS = -ggdb -O0 -export-dynamic
kkfilemanagerdebug_CXXFLAGS = -ggdb -O0 -export-dynamic

bin_PROGRAMS = kkfilemanagerdebug

install-exec-local:
	cd $(DESTDIR)$(bindir) && \
	rm kkfilemanagerdebug

