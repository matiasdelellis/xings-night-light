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

#include "xsct.h"

#define GSETTINGS_SCHEMA "org.xings.night-light"

#define GSETTINGS_KEY_NIGHT_LIGHT_ENABLED "night-light-enabled"
#define GSETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE "night-light-temperature"

static void
xings_night_light_daemon_apply_settings (GSettings *settings)
{
	gboolean enabled;
	guint night_temp;

	enabled = g_settings_get_boolean (settings, GSETTINGS_KEY_NIGHT_LIGHT_ENABLED);
	night_temp = g_settings_get_uint (settings, GSETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE);

	if (enabled) {
		x11_set_temperature (night_temp);
	} else {
		x11_set_temperature (TEMPERATURE_NORM);
	}
}

static void
xings_night_light_daemon_settings_changed_cb (GSettings   *settings,
                                              const gchar *key,
                                              GObject     *object)
{
	xings_night_light_daemon_apply_settings (settings);
}


static gboolean
xings_night_light_daemon_sigint_cb (gpointer user_data)
{
	GMainLoop *loop = user_data;
	g_debug ("Handling SIGINT");
	g_main_loop_quit (loop);
	return FALSE;
}

static gboolean
xings_night_light_daemon_sigterm_cb (gpointer user_data)
{
	GMainLoop *loop = user_data;
	g_debug ("Handling SIGTERM");
	g_main_loop_quit (loop);
	return FALSE;
}


int
main (int   argc,
      char *argv[])
{
	GMainLoop *mainloop = NULL;
	GSettings *settings = NULL;
	GOptionContext *context;
	GError *error = NULL;

	/* Translation */

	setlocale (LC_ALL, "");
	bindtextdomain (GETTEXT_PACKAGE, XINGS_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	/* Parse command line */

	context = g_option_context_new ("- Xings Night Light Daemon");
	if (!g_option_context_parse (context, &argc, &argv, &error))
	{
		g_print ("option parsing failed: %s\n", error->message);
		exit (1);
	}

	/* Settings */

	settings = g_settings_new (GSETTINGS_SCHEMA);
	g_signal_connect (settings, "changed",
	                  G_CALLBACK (xings_night_light_daemon_settings_changed_cb), NULL);

	xings_night_light_daemon_apply_settings (settings);

	/* clean exit on SIGINT or SIGTERM signals */

	mainloop = g_main_loop_new (NULL, FALSE);
	g_unix_signal_add_full (G_PRIORITY_DEFAULT,
	                        SIGINT,
	                        xings_night_light_daemon_sigint_cb,
	                        mainloop,
	                        NULL);
	g_unix_signal_add_full (G_PRIORITY_DEFAULT,
	                        SIGTERM,
	                        xings_night_light_daemon_sigterm_cb,
	                        mainloop,
	                        NULL);

	/* while (TRUE); */
	g_main_loop_run (mainloop);

	/* Clean */

	g_object_unref (settings);
	g_main_loop_unref (mainloop);

	return 0;
}
