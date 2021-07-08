EXTRA_DIST =						\
	xings-night-light-daemon.sgml			\
	xings-night-light-preferences.sgml

if HAVE_DOCBOOK2MAN
man_MANS =						\
	xings-night-light-daemon.1			\
	xings-night-light-preferences.1
endif

if HAVE_DOCBOOK2MAN
xings-night-light-daemon.1: xings-night-light-daemon.sgml
	$(AM_V_GEN) docbook2man $? > /dev/null
xings-night-light-preferences.1: xings-night-light-preferences.sgml
	$(AM_V_GEN) docbook2man $? > /dev/null
endif

clean-local :
	rm -f *~
	rm -f *.1
	rm -f manpage.*
