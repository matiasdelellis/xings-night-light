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

#include "xsct.h"

#include "xnl-common.h"

#include "xnl-smoother.h"

struct _XnlSmoother {
	GObject           _parent;

	GTimer            *smooth_timer;
	guint              smooth_id;

	gdouble            cached_temperature;
	gdouble            target_temperature;
};

#define XNL_SMOOTHER_SMOOTH_SMEAR             5.f       /* seconds */
#define XNL_TEMPERATURE_MAX_DELTA            (10.f)     /* Kelvin */

G_DEFINE_TYPE (XnlSmoother, xnl_smoother, G_TYPE_OBJECT);


static void
xnl_smoother_set_temperature_internal (XnlSmoother *smoother, gdouble temperature);


static gboolean
xnl_smoother_smooth_cb (gpointer user_data)
{
	XnlSmoother *smoother = XNL_SMOOTHER (user_data);
	gdouble tmp;
	gdouble frac;

	/* find fraction */
	frac = g_timer_elapsed (smoother->smooth_timer, NULL) / XNL_SMOOTHER_SMOOTH_SMEAR;
	if (frac >= 1.f) {
		xnl_smoother_set_temperature_internal (smoother,
		                                   smoother->target_temperature);
		smoother->smooth_id = 0;
		return G_SOURCE_REMOVE;
	}

	/* set new temperature step using log curve */
	tmp = smoother->target_temperature - smoother->cached_temperature;
	tmp *= frac;
	tmp += smoother->cached_temperature;

	xnl_smoother_set_temperature_internal (smoother, tmp);

	return G_SOURCE_CONTINUE;
}

static void
xnl_smoother_destroy_smooth (XnlSmoother *smoother)
{
	if (smoother->smooth_id != 0) {
		g_source_remove (smoother->smooth_id);
		smoother->smooth_id = 0;
	}

	if (smoother->smooth_timer != NULL)
		g_clear_pointer (&smoother->smooth_timer, g_timer_destroy);
}

static void
xnl_smoother_create_smooth (XnlSmoother *smoother, gdouble temperature)
{
	g_assert (smoother->smooth_id == 0);
	smoother->target_temperature = temperature;
	smoother->smooth_timer = g_timer_new ();
	smoother->smooth_id = g_timeout_add (50, xnl_smoother_smooth_cb, smoother);
}

static void
xnl_smoother_set_temperature_internal (XnlSmoother *smoother, gdouble temperature)
{
	if (ABS (smoother->cached_temperature - temperature) <= XNL_TEMPERATURE_MAX_DELTA)
		return;

	smoother->cached_temperature = temperature;

	x11_set_display_temperature ((guint) temperature);
}

void
xnl_smoother_set_temperature_sync (XnlSmoother *smoother, gdouble temperature)
{
	xnl_smoother_destroy_smooth (smoother);

	smoother->cached_temperature = temperature;

	x11_set_display_temperature ((guint) temperature);
}


void
xnl_smoother_set_temperature (XnlSmoother *smoother, gdouble temperature)
{
	xnl_smoother_destroy_smooth (smoother);

	if (ABS (temperature - smoother->cached_temperature) < XNL_TEMPERATURE_MAX_DELTA) {
		xnl_smoother_set_temperature_internal (smoother, temperature);
		return;
	}

	xnl_smoother_create_smooth (smoother, temperature);
}

gdouble
xnl_smoother_get_temperature (XnlSmoother *smoother)
{
	return smoother->cached_temperature;
}

static void
xnl_smoother_finalize (GObject *object)
{
	XnlSmoother *smoother = XNL_SMOOTHER (object);

	xnl_smoother_destroy_smooth (smoother);

	G_OBJECT_CLASS (xnl_smoother_parent_class)->finalize (object);
}

static void
xnl_smoother_class_init (XnlSmootherClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = xnl_smoother_finalize;
}

static void
xnl_smoother_init (XnlSmoother *smoother)
{
	smoother->cached_temperature = XNL_COLOR_TEMPERATURE_DEFAULT;
	smoother->target_temperature = XNL_COLOR_TEMPERATURE_DEFAULT;
}

XnlSmoother *
xnl_smoother_new (void)
{
	return g_object_new (XNL_TYPE_SMOOTHER, NULL);
}

