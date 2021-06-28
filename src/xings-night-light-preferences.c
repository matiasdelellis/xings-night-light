/*************************************************************************/
/* Copyright (C) 2021 matias <mati86dl@gmail.com>                        */
/*                                                                       */
/* This program is free software: you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation, either version 3 of the License, or     */
/* (at your option) any later version.                                   */
/*                                                                       */
/* This program is distributed in the hope that it will be useful,       */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */
/*************************************************************************/


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <glib-unix.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#define GSETTINGS_SCHEMA "org.xings.night-light"

#define GSETTINGS_KEY_NIGHT_LIGHT_ENABLED "night-light-enabled"
#define GSETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE "night-light-temperature"

static void
xings_night_light_preferences_settings_changed_cb (GSettings   *settings,
                                                   const gchar *key,
                                                   GtkBuilder  *builder)
{
	GtkWidget *widget;
	gboolean enabled;
	guint night_temp;

	if (g_strcmp0 (key, GSETTINGS_KEY_NIGHT_LIGHT_ENABLED) == 0) {
			enabled = g_settings_get_boolean (settings, GSETTINGS_KEY_NIGHT_LIGHT_ENABLED);
		widget = GTK_WIDGET (gtk_builder_get_object (builder, "switch_enable"));
		gtk_switch_set_active (GTK_SWITCH (widget), enabled);
	}
	else if (g_strcmp0 (key, GSETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE) == 0) {
		night_temp = g_settings_get_uint (settings, GSETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE);
		widget = GTK_WIDGET (gtk_builder_get_object (builder, "scale_strength"));
		gtk_range_set_value (GTK_RANGE (widget), night_temp);
	}
}

static void
xings_night_light_enabled_activated_cb (GtkSwitch  *widget,
                                        GParamSpec *pspec,
                                        GSettings  *settings)
{
	g_settings_set_boolean (settings,
	                        GSETTINGS_KEY_NIGHT_LIGHT_ENABLED,
	                        gtk_switch_get_active (widget));
}

void
xings_night_light_temperature_changed_cb (GtkRange  *range,
                                          GSettings *settings)
{
	g_settings_set_uint (settings,
	                     GSETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE,
	                     gtk_range_get_value(range));
}

static void
xings_night_light_application_activate (GtkApplication *application,
                                        GSettings      *settings)
{
	GtkBuilder *builder;
	GtkWidget *window, *widget;
	guint night_temp;
	gboolean enabled;

	window = gtk_application_window_new (application);
	gtk_window_set_title (GTK_WINDOW (window), _("Night light preferences"));
	gtk_window_set_icon_name (GTK_WINDOW (window), "xings-night-light");
	gtk_window_set_default_size (GTK_WINDOW (window), 500, 300);

	builder = gtk_builder_new_from_file(PKGDATADIR "/xnl-preferences.ui");

	/* Main widget */

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "xnl-preferences"));
	gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (widget));

	/* Init widgets and connect signals */

	g_signal_connect (settings, "changed",
	                  G_CALLBACK (xings_night_light_preferences_settings_changed_cb), builder);

	enabled = g_settings_get_boolean (settings, GSETTINGS_KEY_NIGHT_LIGHT_ENABLED);
	widget = GTK_WIDGET (gtk_builder_get_object (builder, "switch_enable"));
	gtk_switch_set_active (GTK_SWITCH (widget), enabled);
	g_signal_connect (widget, "notify::active",
	                  G_CALLBACK (xings_night_light_enabled_activated_cb), settings);

	night_temp = g_settings_get_uint (settings, GSETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE);
	widget = GTK_WIDGET (gtk_builder_get_object (builder, "scale_strength"));
	gtk_range_set_value (GTK_RANGE (widget), night_temp);
	g_signal_connect (widget, "value-changed",
	                  G_CALLBACK (xings_night_light_temperature_changed_cb), settings);

	gtk_widget_show_all (window);
}

int
main (int    argc,
      char **argv)
{
	GtkApplication *app;
	GSettings *settings;
	int status;

	/* Translation */

	setlocale (LC_ALL, "");
	bindtextdomain (GETTEXT_PACKAGE, XINGS_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	/* Settings */

	settings = g_settings_new (GSETTINGS_SCHEMA);

	/* GtkApplication */

	app = gtk_application_new ("org.xings.night-light-preferences", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (xings_night_light_application_activate), settings);

	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}
