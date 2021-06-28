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

#ifndef __XNL_PREFERENCES_PANEL_H
#define __XNL_PREFERENCES_PANEL_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define XNL_TYPE_PREFERENCES_PANEL (xnl_preferences_panel_get_type())
G_DECLARE_FINAL_TYPE (XnlPreferencesPanel, xnl_preferences_panel, XNL, PREFERENCES_PANEL, GtkBox)

GType xnl_preferences_panel_get_type (void);

XnlPreferencesPanel *xnl_preferences_panel_new (void);

G_END_DECLS

#endif /* __XNL_PREFERENCES_PANEL_H */
