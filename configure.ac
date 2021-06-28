AC_PREREQ(2.60)
AC_INIT([xings-night-light], [0.2.0], [mati86dl@gmail.com])
AM_INIT_AUTOMAKE([1.9.6 -Wall -Werror dist-bzip2])

AC_PROG_CC
# Compiling sources with per-target flags requires AM_PROG_CC_C_O
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(X11, [x11])
PKG_CHECK_MODULES(XRAND, [xrandr])
PKG_CHECK_MODULES(GLIB, [glib-2.0])
PKG_CHECK_MODULES(GIO, [gio-2.0])
PKG_CHECK_MODULES(GTK, [gtk+-3.0])

# Translation

AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.50.1])

GETTEXT_PACKAGE=xings-night-light
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package.])

XDT_I18N([es_AR])

# Enable gsettings schema macros
GLIB_GSETTINGS

#
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	data/Makefile
	data/icons/Makefile
	data/icons/scalable/Makefile
	po/Makefile.in
	src/Makefile
])
AC_OUTPUT
