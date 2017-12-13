#ifndef __APPLET_DATA_H__
#define __APPLET_DATA_H__

#include <ukui-panel-applet.h>

typedef struct _AppletData {
    UkuiPanelApplet *applet;
    GtkWidget *box;
    GtkOrientation orientation;
} AppletData;

#endif // __APPLET_DATA_H__
