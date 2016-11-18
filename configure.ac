#
#
AC_INIT([KKFileManager],[0.0.2],[PROJ],[KKFileManager])

AC_CONFIG_HEADER([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AM_PROG_CC_C_O

CXXFLAGS=-Wall
AC_SUBST(CXXFLAGS)

AC_MSG_CHECKING(whether to use gtk3)
AC_ARG_ENABLE(gtk3,
    [  --enable-gtk3           Use GTK3.],
    [enable_gtk3=$enableval],
    [enable_gtk3="no"])
AC_MSG_RESULT($enable_gtk3)
if test "$enable_gtk3" = "yes"; then
	AC_DEFINE([_USEGTK3_], [], [Use GTK3])
	AC_DEFINE([_EXECSUFFIX_], ["-3"], [Use GTK3])
	GTK_MODULES="gtk+-3.0 >= 3.10.0"
	GTKSRC_MODULES="gtksourceview-3.0 >= 3.10.0"
	program_transform_name='s,\(^[[kK]].*t.*\),\1-3,'
else
	GTK_MODULES="gtk+-2.0 >= 2.24.0"
	GTKSRC_MODULES="gtksourceview-2.0 >= 2.9.9"
	AC_DEFINE([_EXECSUFFIX_], [""], [Use GTK3])
fi

PKG_CHECK_MODULES(GTK, $GTK_MODULES)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AC_OUTPUT(Makefile KKFileManager/app/Makefile)

