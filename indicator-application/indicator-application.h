/*
 * Copyright (C) 2016, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __INDICATOR_APPLICATION_H__
#define __INDICATOR_APPLICATION_H__

#include "../applet-data.h"

#include "na-tray.h"

class IndicatorApplication
{
public:
    IndicatorApplication(AppletData *ad);
    ~IndicatorApplication();

    MatePanelApplet *applet;
    GtkWidget *event_box;
    GtkWidget *alignment;
    GtkOrientation orientation;
    NaTray *tray;
    const int icon_size;

    gulong change_size_handler_id;
    gulong change_background_handler_id;
};

#endif // __INDICATOR_APPLICATION_H__
