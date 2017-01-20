#ifndef __ABSTRACT_INDICATOR_H__
#define __ABSTRACT_INDICATOR_H__

#include <ukui-panel-applet.h>
#include <gtk/gtk.h>

typedef struct _AppletData {
    UkuiPanelApplet *applet;
    GtkWidget *box;
    GtkOrientation orientation;
} AppletData;

#endif // __ABSTRACT_INDICATOR_H__
