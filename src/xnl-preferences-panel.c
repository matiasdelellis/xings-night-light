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

#include "xnl-common.h"

#include "xnl-preferences-panel.h"


struct _XnlPreferencesPanel
{
	GtkBox		_parent;

	GtkBuilder			*builder;
	GSettings			*settings;
};

G_DEFINE_TYPE (XnlPreferencesPanel, xnl_preferences_panel, GTK_TYPE_BOX)


static void
xnl_preferences_panel_settings_changed_cb (GSettings           *settings,
                                           const gchar         *key,
                                           XnlPreferencesPanel *panel)
{
	GtkWidget *widget;
	gboolean enabled;
	guint night_temp;

	if (g_strcmp0 (key, XNL_SETTINGS_KEY_NIGHT_LIGHT_ENABLED) == 0) {
		enabled = g_settings_get_boolean (panel->settings, XNL_SETTINGS_KEY_NIGHT_LIGHT_ENABLED);
		widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "switch_enable"));
		gtk_switch_set_active (GTK_SWITCH (widget), enabled);
	}
	else if (g_strcmp0 (key, XNL_SETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE) == 0) {
		night_temp = g_settings_get_uint (panel->settings, XNL_SETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE);
		widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "scale_strength"));
		gtk_range_set_value (GTK_RANGE (widget), night_temp);
	}
}

static void
xnl_preferences_panel_enabled_activated_cb (GtkSwitch           *widget,
                                            GParamSpec          *pspec,
                                            XnlPreferencesPanel *panel)
{
	g_settings_set_boolean (panel->settings,
	                        XNL_SETTINGS_KEY_NIGHT_LIGHT_ENABLED,
	                        gtk_switch_get_active (widget));
}

void
xnl_preferences_panel_temperature_changed_cb (GtkRange            *range,
                                              XnlPreferencesPanel *panel)
{
	g_settings_set_uint (panel->settings,
	                     XNL_SETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE,
	                     gtk_range_get_value(range));
}

/**
 * xnl_preferences_panel_finalize:
 * @object: The object to finalize
 **/
static void
xnl_preferences_panel_finalize (GObject *object)
{
	XnlPreferencesPanel *panel;

	g_return_if_fail (XNL_IS_PREFERENCES_PANEL (object));

	panel = XNL_PREFERENCES_PANEL (object);

	g_object_unref (panel->settings);
	g_object_unref (panel->builder);

	G_OBJECT_CLASS (xnl_preferences_panel_parent_class)->finalize (object);
}

/**
 * xnl_preferences_panel_init:
 * @preferences_panel: This class instance
 **/
static void
xnl_preferences_panel_init (XnlPreferencesPanel *panel)
{
	GtkWidget *widget;
	guint night_temp;
	gboolean enabled;

	panel->builder = gtk_builder_new_from_file(PKGDATADIR "/xnl-preferences.ui");

	/* Main widget */

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "xnl-preferences"));
	gtk_box_pack_start (GTK_BOX (panel), GTK_WIDGET (widget), TRUE, TRUE, 0);

	/* Init widgets and connect signals */

	panel->settings = g_settings_new (XNL_SETTINGS_SCHEMA);
	g_signal_connect (panel->settings, "changed",
	                  G_CALLBACK (xnl_preferences_panel_settings_changed_cb), panel);

	enabled = g_settings_get_boolean (panel->settings, XNL_SETTINGS_KEY_NIGHT_LIGHT_ENABLED);
	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "switch_enable"));
	gtk_switch_set_active (GTK_SWITCH (widget), enabled);
	g_signal_connect (widget, "notify::active",
	                  G_CALLBACK (xnl_preferences_panel_enabled_activated_cb), panel);

	night_temp = g_settings_get_uint (panel->settings, XNL_SETTINGS_KEY_NIGHT_LIGHT_TEMPERATURE);
	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "scale_strength"));
	gtk_range_set_value (GTK_RANGE (widget), night_temp);
	g_signal_connect (widget, "value-changed",
	                  G_CALLBACK (xnl_preferences_panel_temperature_changed_cb), panel);
}

/**
 * xnl_preferences_panel_class_init:
 * @klass: The XnlPreferencesPanelClass
 **/
static void
xnl_preferences_panel_class_init (XnlPreferencesPanelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = xnl_preferences_panel_finalize;
}

/**
 * xnl_preferences_panel_new:
 *
 * Return value: a new XnlPreferencesPanel object.
 **/
XnlPreferencesPanel *
xnl_preferences_panel_new (void)
{
	XnlPreferencesPanel *preferences_panel;
	preferences_panel = g_object_new (XNL_TYPE_PREFERENCES_PANEL, NULL);
	return XNL_PREFERENCES_PANEL (preferences_panel);
}

