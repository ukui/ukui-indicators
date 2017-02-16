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
