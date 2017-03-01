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

#include "indicator-desktop.h"

#include <gdk/gdkx.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#undef WNCK_I_KNOW_THIS_IS_UNSTABLE

class IndicatorDesktop;

static void show_desktop_changed_cb(WnckScreen* screen, IndicatorDesktop *d);
static gboolean on_button_press(GtkWidget* widget, GdkEventButton* event, IndicatorDesktop *d);
static gboolean on_button_release(GtkWidget* widget, GdkEventButton* event, IndicatorDesktop *d);
static gboolean button_hover_callback(GtkWidget *event_box, GdkEvent *event, IndicatorDesktop *d);
static void applet_size_changed(MatePanelApplet *applet, guint size, IndicatorDesktop *d);

static const double WidthHeightRatio = 0.4;
static const int HoverDelay = 400; // wait for 400ms to trigger showing desktop
static WnckScreen *wnck_screen = NULL;

IndicatorDesktop::IndicatorDesktop(AppletData *ad) :
    applet(ad->applet),
    image(NULL),
    orientation(ad->orientation),
    has_cursor(FALSE),
    hover_blocked(FALSE),
    showing_desktop(FALSE)
{
    size = mate_panel_applet_get_size(applet);
    change_size_handler_id = g_signal_connect(applet, "change_size",
                                              G_CALLBACK(applet_size_changed), this);

    if (wnck_screen == NULL)
        wnck_screen = wnck_screen_get_default();

    if (wnck_screen)
        show_desktop_changed_handle_id = g_signal_connect(wnck_screen, "showing-desktop-changed",
                                                          G_CALLBACK(show_desktop_changed_cb), this);
    else
        g_warning("Could not get WnckScreen");
    show_desktop_changed_cb(wnck_screen, this);

    image = gtk_image_new_from_pixbuf(NULL);
    set_button_state(StateNormal);

    g_signal_connect(applet, "change_size", G_CALLBACK(applet_size_changed), this);

    event_box = gtk_event_box_new();
    g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_button_press), this);
    g_signal_connect(event_box, "button-release-event", G_CALLBACK(on_button_release), this);
    g_signal_connect(event_box, "enter-notify-event", G_CALLBACK(button_hover_callback), this);
    g_signal_connect(event_box, "leave-notify-event", G_CALLBACK(button_hover_callback), this);

//    gtk_widget_set_tooltip_text(event_box, "显示桌面");
    gtk_container_add(GTK_CONTAINER(event_box), image);
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(event_box), FALSE);
    gtk_widget_show_all(event_box);
}

IndicatorDesktop::~IndicatorDesktop()
{
    g_signal_handler_disconnect(wnck_screen, show_desktop_changed_handle_id);
    g_signal_handler_disconnect(applet, change_size_handler_id);
}

void IndicatorDesktop::set_button_state(AppletState state)
{
    GdkPixbuf *pixbuf = NULL;
    GdkPixbuf *scaled = NULL;

    if (image == NULL)
        return;

    if (state == StateHovered)
        pixbuf = gdk_pixbuf_new_from_file(PIXMAPS_DIR"/show-desktop-hovered.png", NULL);
    else if (state == StatePressed)
        pixbuf = gdk_pixbuf_new_from_file(PIXMAPS_DIR"/show-desktop-pressed.png", NULL);
    else
        pixbuf = gdk_pixbuf_new_from_file(PIXMAPS_DIR"/show-desktop-normal.png", NULL);

    if (orientation == GTK_ORIENTATION_HORIZONTAL) {
        scaled = gdk_pixbuf_scale_simple(pixbuf, size * WidthHeightRatio, size, GDK_INTERP_BILINEAR);
        gtk_widget_set_size_request(image, size * WidthHeightRatio, size);
    } else {
        scaled = gdk_pixbuf_scale_simple(pixbuf, size, size * WidthHeightRatio, GDK_INTERP_BILINEAR);
        gtk_widget_set_size_request(image, size, size * WidthHeightRatio);
    }

    gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled);

    g_object_unref(scaled);
    g_object_unref(pixbuf);
}

static void
show_desktop_changed_cb(WnckScreen* screen, IndicatorDesktop *d)
{
    if (wnck_screen)
        d->showing_desktop = wnck_screen_get_showing_desktop(wnck_screen);

    if (d->showing_desktop == FALSE) {
        d->set_button_state(StateNormal);
        d->hover_blocked = FALSE;
    }
}

static void
show_desktop(IndicatorDesktop *d, gboolean show)
{
    static GtkWidget* dialog = NULL;

    GtkWidget *button = d->event_box;

    if (!gdk_x11_screen_supports_net_wm_hint(gtk_widget_get_screen(button), gdk_atom_intern("_NET_SHOWING_DESKTOP", FALSE))) {
        if (dialog && gtk_widget_get_screen(dialog) != gtk_widget_get_screen(button))
            gtk_widget_destroy (dialog);

        if (dialog) {
            gtk_window_present(GTK_WINDOW(dialog));
            return;
        }

        dialog = gtk_message_dialog_new(NULL,
                                        GTK_DIALOG_MODAL,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        "显示桌面出错，请检查窗口管理器是否支持_NET_SHOWING_DESKTOP");

        g_object_add_weak_pointer(G_OBJECT(dialog), (gpointer *) &dialog);

        g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(gtk_widget_destroy), NULL);

        gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
        gtk_window_set_screen(GTK_WINDOW(dialog), gtk_widget_get_screen(button));
        gtk_widget_show(dialog);

        return;
    }

    if (wnck_screen)
        wnck_screen_toggle_showing_desktop(wnck_screen, show);
}

static gboolean
on_button_press(GtkWidget* widget, GdkEventButton* event, IndicatorDesktop *d)
{
    if (event->button != 1)
        return TRUE;

    d->set_button_state(StatePressed);

    if (d->showing_desktop) {
        if (d->hover_blocked)
            show_desktop(d, FALSE);
        else
            d->hover_blocked = TRUE;
    } else {
        show_desktop(d, TRUE);
        d->hover_blocked = TRUE;
    }

    return FALSE;
}

static void
applet_size_changed(MatePanelApplet *applet, guint size, IndicatorDesktop *d)
{
    d->size = size;
    d->set_button_state(StateNormal);
}

static gboolean
on_button_release(GtkWidget* widget, GdkEventButton* event, IndicatorDesktop *d)
{
    if (event->button != 1)
        return FALSE;

    if (d->hover_blocked)
        return FALSE;

    d->set_button_state(StateHovered);

    return FALSE;
}

static gboolean
timeout_callback(IndicatorDesktop *d)
{
    if (d->has_cursor == FALSE) {
        return FALSE;
    }
    show_desktop(d, TRUE);

    // always return FALSE since this should be a single shot timer
    return FALSE;
}

static gboolean
button_hover_callback(GtkWidget *event_box, GdkEvent *event, IndicatorDesktop *d)
{
    if (d->hover_blocked)
        return FALSE;

    if (event->type == GDK_ENTER_NOTIFY) {
        d->set_button_state(StateHovered);

        d->has_cursor = TRUE;
        g_timeout_add(HoverDelay, GSourceFunc(timeout_callback), d);
    }
    if (event->type == GDK_LEAVE_NOTIFY) {
        d->set_button_state(StateNormal);

        d->has_cursor = FALSE;
        show_desktop(d, FALSE);
    }

    return FALSE;
}

