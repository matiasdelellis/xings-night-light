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

#ifndef __XNL_SMOOTHER_H__
#define __XNL_SMOOTHER_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define XNL_TYPE_SMOOTHER (xnl_smoother_get_type ())
G_DECLARE_FINAL_TYPE (XnlSmoother, xnl_smoother, XNL, SMOOTHER, GObject)

XnlSmoother *xnl_smoother_new                  (void);

void         xnl_smoother_set_temperature_sync (XnlSmoother *smoother, gdouble temperature);
void         xnl_smoother_set_temperature      (XnlSmoother *smoother, gdouble temperature);
gdouble      xnl_smoother_get_temperature      (XnlSmoother *smoother);

G_END_DECLS

#endif
