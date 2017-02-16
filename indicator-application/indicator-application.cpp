#include "indicator-application.h"

#include <gtk/gtk.h>
#include <mate-panel-applet.h>
#include <string.h>
#include <cairo.h>


static void applet_change_background(MatePanelApplet* applet, MatePanelAppletBackgroundType type, GdkColor* color, cairo_pattern_t *pattern, IndicatorApplication *d);
static void applet_size_changed(MatePanelApplet* applet, int size, IndicatorApplication *d);
static void force_no_focus_padding(GtkWidget* widget);


IndicatorApplication::IndicatorApplication(AppletData *ad) :
    applet(ad->applet),
    orientation(ad->orientation),
    icon_size(16)
{
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

    applet_size_changed(applet, mate_panel_applet_get_size(applet), this);
}

IndicatorApplication::~IndicatorApplication()
{
    g_signal_handler_disconnect(applet, change_background_handler_id);
    g_signal_handler_disconnect(applet, change_size_handler_id);
}

static void applet_size_changed(MatePanelApplet* applet, int size, IndicatorApplication *d)
{
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
            gtk_alignment_set_padding(GTK_ALIGNMENT(d->alignment), 0, 0, 0, 0);
        else
            gtk_alignment_set_padding(GTK_ALIGNMENT(d->alignment), 0, 0, padding1, padding2);
    }
}

static void
applet_change_background(MatePanelApplet* applet,
                         MatePanelAppletBackgroundType type,
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

    if (first_time)
    {
        gtk_rc_parse_string ("\n"
                             "style \"na-tray-style\"\n"
                             "{\n"
                             "    GtkWidget::focus-line-width=0\n"
                             "    GtkWidget::focus-padding=0\n"
                             "}\n"
                             "\n"
                             "    widget \"*.PanelAppletNaTray\" style \"na-tray-style\"\n"
                             "\n");

        first_time = FALSE;
    }

    /* The widget used to be called na-tray
     *
     * Issue #27
     */
    gtk_widget_set_name(widget, "PanelAppletNaTray");
}
