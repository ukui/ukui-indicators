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

#ifndef __INDICATOR_CALENDAR_H__
#define __INDICATOR_CALENDAR_H__

#include "../applet-data.h"

#include <string>

class IndicatorCalendar
{
public:
    IndicatorCalendar(AppletData *ad);
    ~IndicatorCalendar();

    MatePanelApplet *applet;

    GSettings *settings;
    GSettings *panel_settings;
    guint time_source;

    GtkWidget *main_window;
    GtkWidget *webview;

    GtkWidget *applet_button;
    GtkWidget *applet_label;
    GtkWidget *event_box;

    GtkOrientation orientation;
    GtkWidget *menu;

    gboolean has_cursor;
    gboolean button_pressed;

    gboolean use_24h_format;
    gboolean show_second;
    std::string time; // e.g., 19:48:02 or 19:48
    std::string theme;


private:
    void _setup_main_window();
    void _setup_popup_menu();
    void _parse_rc();
};

#endif // __INDICATOR_CALENDAR_H__
