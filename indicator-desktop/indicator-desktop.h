/*
 *  Copyright (C) 2016, Tianjin KYLIN Information Technology Co., Ltd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef __INDICATOR_DESKTOP_H__
#define __INDICATOR_DESKTOP_H__

#include "../applet-data.h"

enum AppletState { StateNormal, StateHovered, StatePressed };

class IndicatorDesktop
{
public:
    IndicatorDesktop(AppletData *ad);
    ~IndicatorDesktop();

    void set_button_state(AppletState state);

    MatePanelApplet *applet;
    GtkWidget *event_box;
    GtkWidget *image;
    GtkOrientation orientation;
    guint size;

    gulong show_desktop_changed_handle_id;
    gulong change_size_handler_id;
    gboolean has_cursor;
    gboolean hover_blocked;
    gboolean showing_desktop;
};

#endif // __INDICATOR_DESKTOP_H__
