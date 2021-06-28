SUBDIRS = icons

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@
gsettingsschema_in_files = org.xings.night-light.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)

@INTLTOOL_DESKTOP_RULE@
desktopdir = $(datadir)/applications
desktop_in_files = xings-night-light-preferences.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@
autostartdir = $(sysconfdir)/xdg/autostart
autostart_in_files = xings-night-light-daemon.desktop.in
autostart_DATA = $(autostart_in_files:.desktop.in=.desktop)

pkgdata_DATA = \
	xnl-preferences.ui

EXTRA_DIST= \
	$(gsettingsschema_in_files) \
	$(desktop_in_files) \
	$(autostart_in_files) \
	$(pkgdata_DATA)

CLEANFILES = \
	$(gsettings_SCHEMAS) \
	$(desktop_DATA) \
	$(autostart_DATA)