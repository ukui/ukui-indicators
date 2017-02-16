#ifndef __ABSTRACT_INDICATOR_H__
#define __ABSTRACT_INDICATOR_H__

#include <mate-panel-applet.h>
#include <gtk/gtk.h>

typedef struct _AppletData {
    MatePanelApplet *applet;
    GtkWidget *box;
    GtkOrientation orientation;
} AppletData;

#endif // __ABSTRACT_INDICATOR_H__
