#ifndef __APPLET_DATA_H__
#define __APPLET_DATA_H__

#include <mate-panel-applet.h>

typedef struct _AppletData {
    MatePanelApplet *applet;
    GtkWidget *box;
    GtkOrientation orientation;
} AppletData;

#endif // __APPLET_DATA_H__
