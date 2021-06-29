bin_PROGRAMS = \
	xings-night-light-daemon			\
	xings-night-light-preferences

AM_CPPFLAGS = \
	-I$(top_srcdir)					\
	-I$(top_srcdir)/src				\
	-DPKGDATADIR=\"$(pkgdatadir)\"			\
	-DG_LOG_DOMAIN=\"xings-night-light-daemon\"	\
	-DXINGS_LOCALE_DIR=\""$(datadir)/locale"\"

xings_night_light_daemon_SOURCES =			\
	xings-night-light-daemon.c			\
	xnl-daemon.c					\
	xnl-daemon.h					\
	xnl-common.h					\
	xnl-debug.c					\
	xnl-debug.h					\
	xsct.c						\
	xsct.h

xings_night_light_daemon_CFLAGS =			\
	$(X11_CFLAGS)					\
	$(XRAND_CFLAGS)					\
	$(GIO_CFLAGS)					\
	$(GLIB_CFLAGS)

xings_night_light_daemon_LDADD =			\
	$(X11_LIBS)					\
	$(XRAND_LIBS)					\
	$(GIO_LIBS)					\
	$(GLIB_LIBS) -lm

xings_night_light_preferences_SOURCES =			\
	xings-night-light-preferences.c			\
	xnl-preferences-panel.c				\
	xnl-preferences-panel.h				\
	xnl-common.h					\
	xnl-debug.c					\
	xnl-debug.h

xings_night_light_preferences_CFLAGS =			\
	$(GLIB_CFLAGS)					\
	$(GIO_CFLAGS)					\
	$(GTK_CFLAGS)

xings_night_light_preferences_LDADD =			\
	$(GLIB_LIBS)					\
	$(GIO_LIBS)					\
	$(GTK_LIBS)

CLEANFILES = *~
