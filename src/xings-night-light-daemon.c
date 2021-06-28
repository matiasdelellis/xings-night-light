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

#include "xnl-common.h"
#include "xnl-debug.h"
#include "xnl-daemon.h"

static gboolean
xings_night_light_daemon_sigint_cb (gpointer user_data)
{
	g_debug ("Handling SIGINT");
	g_application_quit (G_APPLICATION (user_data));
	return FALSE;
}

static gboolean
xings_night_light_daemon_sigterm_cb (gpointer user_data)
{
	g_debug ("Handling SIGTERM");
	g_application_quit (G_APPLICATION (user_data));
	return FALSE;
}

static void
xings_night_light_daemon_activate (GApplication *application)
{
	g_application_hold (application);
}

int
main (int argc, char *argv[])
{
	GApplication *app;
	GOptionContext *context;
	XnlDaemon *daemon;
	int status;

	/* Translation */

	setlocale (LC_ALL, "");
	bindtextdomain (GETTEXT_PACKAGE, XINGS_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	/* Debug options */

	context = g_option_context_new (NULL);
	g_option_context_set_summary (context, _("Xings Night Light Daemon"));
	g_option_context_add_group (context, xnl_debug_get_option_group ());
	g_option_context_parse (context, &argc, &argv, NULL);
	g_option_context_free (context);

	xnl_debug_add_log_domain ("XingsNightLightDaemon");

	/* Xings Night Light Daemon */

	daemon = xnl_daemon_new ();

	/* Create app to handle the public service */

	app = g_application_new ("org.xings.NightLightDaemon",
	                         G_APPLICATION_FLAGS_NONE);

	g_signal_connect (app, "activate",
	                  G_CALLBACK (xings_night_light_daemon_activate), NULL);

	/* clean exit on SIGINT or SIGTERM signals */

	g_unix_signal_add_full (G_PRIORITY_DEFAULT,
	                        SIGINT,
	                        xings_night_light_daemon_sigint_cb,
	                        app,
	                        NULL);
	g_unix_signal_add_full (G_PRIORITY_DEFAULT,
	                        SIGTERM,
	                        xings_night_light_daemon_sigterm_cb,
	                        app,
	                        NULL);

	/* while (TRUE); */
	status = g_application_run (app, argc, argv);

	/* Clean */

	g_object_unref (daemon);
	g_object_unref (app);

	return status;
}
