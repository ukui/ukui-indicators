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

#include "indicator-application.h"

#include <string.h>
#include <cairo.h>


static void applet_change_background(UkuiPanelApplet* applet, UkuiPanelAppletBackgroundType type, GdkColor* color, cairo_pattern_t *pattern, IndicatorApplication *d);
static void applet_size_changed(UkuiPanelApplet* applet, int size, IndicatorApplication *d);
static void force_no_focus_padding(GtkWidget* widget);


IndicatorApplication::IndicatorApplication(AppletData *ad) :
    applet(ad->applet),
    orientation(ad->orientation),
    icon_size(16)
{
    char 	*path, *path1;
    GSettings 	*settings, *settings1;

    tray = na_tray_new_for_screen(gtk_widget_get_screen(GTK_WIDGET(applet)), orientation);
    force_no_focus_padding(GTK_WIDGET(tray));

    alignment = gtk_alignment_new(0.5, 0.5, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignment), 0, 0, 0, 0);
    gtk_container_add(GTK_CONTAINER(alignment), GTK_WIDGET(tray));

    event_box = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(event_box), FALSE);
    gtk_container_add(GTK_CONTAINER(event_box), alignment);

    change_background_handler_id = g_signal_connect(applet, "change_background",
                                                    G_CALLBACK (applet_change_background), this);
    change_size_handler_id = g_signal_connect(applet, "change_size",
                                              G_CALLBACK(applet_size_changed), this);

    applet_size_changed(applet, ukui_panel_applet_get_size(applet), this);

    for(int i = 1; i < 20; i ++){
		path = 			g_strdup_printf ("%s%d/", "/org/ukui/panel/indicator/tray", i);
		settings =              g_settings_new_with_path ("org.ukui.panel.indicator.tray", path);

		g_settings_set_int (settings, "number",0);
    }

    path1 = g_strdup_printf ("%s/","/org/ukui/panel/toplevels/bottom");
    settings1 = g_settings_new_with_path ("org.ukui.panel.toplevel",path1);
    g_settings_set_int(settings1, "launcher-nums",0);

}

IndicatorApplication::~IndicatorApplication()
{
    g_signal_handler_disconnect(applet, change_background_handler_id);
    g_signal_handler_disconnect(applet, change_size_handler_id);
}

static void applet_size_changed(UkuiPanelApplet* applet, int size, IndicatorApplication *d)
{
    switch (size){
	case 40:
		gtk_box_set_spacing (GTK_BOX (d->tray->priv->box), 10);
		d->icon_size = 16;
		break;
	case 60:
		gtk_box_set_spacing (GTK_BOX (d->tray->priv->box), 15);
		d->icon_size = 22;
		break;
	case 80:
		gtk_box_set_spacing (GTK_BOX (d->tray->priv->box), 20);
		d->icon_size = 32;
		break;
	default:
		gtk_box_set_spacing (GTK_BOX (d->tray->priv->box), 10);
		d->icon_size = 16;
    }

    if (d->orientation == GTK_ORIENTATION_HORIZONTAL)
        gtk_widget_set_size_request(d->event_box, -1, size);
    else
        gtk_widget_set_size_request(d->event_box, size, -1);

    if (size < d->icon_size) {
        gtk_alignment_set_padding(GTK_ALIGNMENT(d->alignment), 0, 0, 0, 0);
    } else {
        int padding1 = (size - d->icon_size) / 2;
        int padding2 = size - d->icon_size - padding1;
        if (d->orientation == GTK_ORIENTATION_HORIZONTAL)
            gtk_alignment_set_padding(GTK_ALIGNMENT(d->alignment), padding1, padding2, 0, 0);
        else
            gtk_alignment_set_padding(GTK_ALIGNMENT(d->alignment), 0, 0, padding1, padding2);
    }
}

static void
applet_change_background(UkuiPanelApplet* applet,
                         UkuiPanelAppletBackgroundType type,
                         GdkColor* color,
                         cairo_pattern_t *pattern,
                         IndicatorApplication *d)
{
    g_return_if_fail(d);
    g_return_if_fail(d->tray);
    g_return_if_fail(NA_IS_TRAY(d->tray));

    na_tray_force_redraw(d->tray);
}

static void force_no_focus_padding(GtkWidget* widget)
{
    static gboolean first_time = TRUE;

    GtkCssProvider *provider;

    if (first_time)
    {
        provider = gtk_css_provider_new();

        gtk_css_provider_load_from_data(provider,
                                        "#PanelAppletNaTray {"
                                        "    outline-width : 0px;\n"
                                        "    outline-offset: 0px;\n"
                                        "}", -1, NULL);
        g_object_unref(provider);

        first_time = FALSE;
    }

    /* The widget used to be called na-tray
     *
     * Issue #27
     */
    gtk_widget_set_name(widget, "PanelAppletNaTray");
}
