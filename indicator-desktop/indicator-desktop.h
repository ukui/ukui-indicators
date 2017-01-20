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

    UkuiPanelApplet *applet;
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
