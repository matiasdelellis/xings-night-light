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
#include <glib/gi18n.h>

#include "xnl-smoother.h"
#include "xnl-common.h"

#ifdef HAVE_STATUSNOTIFIER
#include <statusnotifier.h>
#endif

#include "xnl-daemon.h"

struct _XnlDaemon
{
	GObject			 parent;

	XnlSmoother		*smoother;
	GSettings		*settings;

#ifdef HAVE_STATUSNOTIFIER
	StatusNotifierItem	*status_notifier;
#endif
};

G_DEFINE_TYPE (XnlDaemon, xnl_daemon, G_TYPE_OBJECT)


#ifdef HAVE_STATUSNOTIFIER
static void
xnl_daemon_show_applet (XnlDaemon *daemon,
                        gboolean   active)
{
	g_object_set (daemon->status_notifier,
	              "title", active ? _("Turn off night light") : _("Turn on night light"),
	              NULL);
	g_object_set (daemon->status_notifier,
	              "tooltip-title", active ? _("Turn off night light") : _("Turn on night light"),
	              NULL);
	g_object_set (daemon->status_notifier,
	              "main-icon-name", active ? "night-light-symbolic" : "night-light-disabled-symbolic",
	              NULL);

	g_object_set (daemon->status_notifier,
	              "status", STATUS_NOTIFIER_STATUS_ACTIVE, NULL);
}

static void
xnl_daemon_hide_applet (XnlDaemon *daemon)
{
	g_object_set (daemon->status_notifier,
	              "status", STATUS_NOTIFIER_STATUS_PASSIVE,
	              NULL);
}
static void
xnl_daemon_applet_icon_activate_cb (XnlDaemon *daemon)
{
	g_settings_set_boolean (daemon->settings, XNL_SETTINGS_KEY_NIGHT_LIGHT_ENABLED,
		!g_settings_get_boolean (daemon->settings, XNL_SETTINGS_KEY_NIGHT_LIGHT_ENABLED));
}
#endif

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

#ifdef HAVE_STATUSNOTIFIER
	xnl_daemon_show_applet (daemon, enabled);
#endif
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

#ifdef HAVE_STATUSNOTIFIER
	g_clear_object (&daemon->status_notifier);
#endif

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

#ifdef HAVE_STATUSNOTIFIER
	daemon->status_notifier = g_object_new (STATUS_NOTIFIER_TYPE_ITEM,
	                                        "id",           "xings-night-light",
	                                        "category",     STATUS_NOTIFIER_CATEGORY_SYSTEM_SERVICES,
	                                        "status",       STATUS_NOTIFIER_STATUS_PASSIVE,
	                                        "title",        _("Turn on night light"),
	                                        "item-is-menu", FALSE,
	                                        NULL);

	g_signal_connect_swapped (daemon->status_notifier, "activate",
	                          G_CALLBACK(xnl_daemon_applet_icon_activate_cb),
	                          daemon);

	status_notifier_item_register (daemon->status_notifier);
#endif

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

