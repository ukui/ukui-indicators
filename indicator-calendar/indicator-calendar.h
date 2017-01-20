#ifndef __INDICATOR_CALENDAR_H__
#define __INDICATOR_CALENDAR_H__

#include "../applet-data.h"

#include <string>

class IndicatorCalendar
{
public:
    IndicatorCalendar(AppletData *ad);
    ~IndicatorCalendar();

    UkuiPanelApplet *applet;

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
