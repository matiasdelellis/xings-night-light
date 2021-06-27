bin_PROGRAMS = xings-night-light-daemon

AM_CPPFLAGS = \
	-I$(top_srcdir)					\
	-I$(top_srcdir)/src				\
	-DG_LOG_DOMAIN=\"xings-night-light-daemon\"	\
	-DXINGS_LOCALE_DIR=\""$(datadir)/locale"\"

xings_night_light_daemon_SOURCES =			\
	xings-night-light-daemon.c			\
	xsct.c						\
	xsct.h

xings_night_light_daemon_CFLAGS =			\
	$(X11_CFLAGS)					\
	$(XRAND_CFLAGS)					\
	$(GIO_CFLAGS)					\
	$(GLIB_CFLAGS) -lm

xings_night_light_daemon_LDADD =			\
	$(X11_LIBS)					\
	$(XRAND_LIBS)					\
	$(GIO_LIBS)					\
	$(GLIB_LIBS) -lm

CLEANFILES = *~

