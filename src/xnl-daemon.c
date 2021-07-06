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
#include <gio/gio.h>

#include "xnl-smoother.h"
#include "xnl-common.h"

#include "xnl-daemon.h"

struct _XnlDaemon
{
	GObject			 parent;

	XnlSmoother		*smoother;
	GSettings		*settings;
};

G_DEFINE_TYPE (XnlDaemon, xnl_daemon, G_TYPE_OBJECT)

/**
 * xnl_daemon_apply_settings
 **/
static void
xnl_daemon_apply_settings (XnlDaemon *daemon)
{
	gboolean enabled;
	guint night_temp;

	enabled = g_settings_get_boolean (daemon->settings, XNL_SETTINGS_KEY_NIGHT_LIGHT_ENABLED);
	night_temp = g_settings_get_uint (daemon->settings, XNL_SETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE);

	xnl_smoother_set_temperature (daemon->smoother,
	                              enabled ? night_temp : XNL_COLOR_TEMPERATURE_DEFAULT);
}

/**
 * xnl_settings_changed_cb
 **/
static void
xnl_settings_changed_cb (GSettings   *settings,
                         const gchar *key,
                         GObject     *object)
{
	xnl_daemon_apply_settings (XNL_DAEMON(object));
}

/**
 * xnl_daemon_finalize:
 * @object: The object to finalize
 **/
static void
xnl_daemon_finalize (GObject *object)
{
	XnlDaemon *daemon;

	g_return_if_fail (XNL_IS_DAEMON (object));

	daemon = XNL_DAEMON (object);

	xnl_smoother_set_temperature_sync (daemon->smoother, XNL_COLOR_TEMPERATURE_DEFAULT);

	g_object_unref (daemon->smoother);

	g_object_unref (daemon->settings);

	G_OBJECT_CLASS (xnl_daemon_parent_class)->finalize (object);
}

/**
 * xnl_daemon_init:
 * @daemon: This class instance
 **/
static void
xnl_daemon_init (XnlDaemon *daemon)
{
	daemon->smoother = xnl_smoother_new ();

	daemon->settings = g_settings_new (XNL_SETTINGS_SCHEMA);
	g_signal_connect (daemon->settings, "changed",
	                  G_CALLBACK (xnl_settings_changed_cb), daemon);
	xnl_daemon_apply_settings (daemon);
}

/**
 * xnl_daemon_class_init:
 * @klass: The XnlDaemonClass
 **/
static void
xnl_daemon_class_init (XnlDaemonClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = xnl_daemon_finalize;
}

/**
 * xnl_daemon_new:
 *
 * Return value: a new XnlDaemon object.
 **/
XnlDaemon *
xnl_daemon_new (void)
{
	XnlDaemon *daemon;
	daemon = g_object_new (XNL_TYPE_DAEMON, NULL);
	return XNL_DAEMON (daemon);
}

