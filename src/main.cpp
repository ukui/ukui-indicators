#include "indicator-application.h"
#include "indicator-calendar.h"
#include "indicator-desktop.h"

#include <string>

static void
load_content(AppletData *ad)
{
    static IndicatorApplication *apps = NULL;
    static IndicatorCalendar *calendar = NULL;
    static IndicatorDesktop *desktop = NULL;

    if (apps)
        delete apps;
    apps = new IndicatorApplication(ad);

    if (calendar)
        delete calendar;
    calendar = new IndicatorCalendar(ad);

    if (desktop)
        delete desktop;
    desktop = new IndicatorDesktop(ad);

    gtk_box_pack_start(GTK_BOX(ad->box), apps->event_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(ad->box), calendar->event_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(ad->box), desktop->event_box, FALSE, FALSE, 0);
}

static void
orientation_changed(UkuiPanelApplet* applet, UkuiPanelAppletOrient orientation, AppletData *ad)
{
    switch (orientation) {
    case UKUI_PANEL_APPLET_ORIENT_UP:
    case UKUI_PANEL_APPLET_ORIENT_DOWN:
        ad->orientation = GTK_ORIENTATION_HORIZONTAL;
        break;

    case UKUI_PANEL_APPLET_ORIENT_LEFT:
    case UKUI_PANEL_APPLET_ORIENT_RIGHT:
    default:
        ad->orientation = GTK_ORIENTATION_VERTICAL;
        break;
    }

    if (ad->box)
        gtk_widget_destroy(ad->box);

    if (ad->orientation == GTK_ORIENTATION_HORIZONTAL)
        ad->box = gtk_hbox_new(FALSE, 0);
    else
        ad->box = gtk_vbox_new(FALSE, 0);

    load_content(ad);

    gtk_container_add(GTK_CONTAINER(ad->applet), ad->box);
    gtk_widget_show_all(ad->box);
}

static void
on_applet_destroy(GtkWidget *applet, AppletData *ad)
{
    g_free(ad);
}

static void
applet_realized(GtkWidget *applet, gpointer user_data)
{
    AppletData *ad;

    ad = static_cast<AppletData*>(g_malloc0(sizeof(AppletData)));
    ad->applet = UKUI_PANEL_APPLET(applet);

    g_signal_connect(ad->applet, "change-orient", G_CALLBACK(orientation_changed), ad);
    g_signal_connect(ad->applet, "destroy", G_CALLBACK(on_applet_destroy), ad);

    orientation_changed(ad->applet, ukui_panel_applet_get_orient(ad->applet), ad);
}

static
void log_handler(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
    static GtkWidget *debug_window = NULL;
    static GtkWidget *debug_label = NULL;

    if (debug_window == NULL) {
        debug_label = gtk_label_new(NULL);
        gtk_label_set_selectable(GTK_LABEL(debug_label), TRUE);

        GtkWidget *viewport = gtk_viewport_new(NULL, NULL);
        gtk_container_add(GTK_CONTAINER(viewport), debug_label);

        GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_container_add(GTK_CONTAINER(scrolled), viewport);

        debug_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_deletable(GTK_WINDOW(debug_window), FALSE);
        gtk_widget_set_size_request(debug_window, 600, 400);
        gtk_container_add(GTK_CONTAINER(debug_window), scrolled);
        gtk_container_set_border_width(GTK_CONTAINER(debug_window), 10);
        gtk_widget_show_all(debug_window);
    }

    if (message == NULL)
        return;

    const gchar *text = gtk_label_get_label(GTK_LABEL(debug_label));
    gchar *new_text = g_strdup_printf("%s\n%s###%s###", text, log_domain, message);
    gtk_label_set_label(GTK_LABEL(debug_label), new_text);
    g_free(new_text);
}

static gboolean
applet_factory(UkuiPanelApplet *applet, const gchar *iid, gpointer user_data)
{
    if (std::string(iid) != "UkuiIndicatorsApplet")
        return FALSE;

#ifndef NDEBUG
    g_log_set_handler(NULL,
                      static_cast<GLogLevelFlags>(G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION),
                      log_handler, NULL);
#endif

    g_signal_connect(GTK_WIDGET(applet), "realize", G_CALLBACK(applet_realized), NULL);
    ukui_panel_applet_set_flags(applet,
                                static_cast<UkuiPanelAppletFlags>(UKUI_PANEL_APPLET_EXPAND_MINOR | UKUI_PANEL_APPLET_HAS_HANDLE));
    ukui_panel_applet_set_background_widget(applet, GTK_WIDGET(applet));

    gtk_widget_show_all(GTK_WIDGET(applet));

    return TRUE;
}

UKUI_PANEL_APPLET_OUT_PROCESS_FACTORY("UkuiIndicatorsAppletFactory",
                                      PANEL_TYPE_APPLET,
                                      "UkuiIndicatorsApplet",
                                      applet_factory,
                                      NULL)
