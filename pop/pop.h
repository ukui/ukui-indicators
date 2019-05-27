#ifndef __POP_H__
#define __POP_H__

#include "../applet-data.h"

#include "na-tray.h"

class Pop
{
public:
    Pop(AppletData *ad);
    ~Pop();

    UkuiPanelApplet *applet;
    GtkWidget *event_box;
    GtkWidget *alignment;
    
    GtkWidget *button;
    GtkWidget *image;

    GtkWidget *main_window; 
    GtkOrientation orientation;
    NaTray *tray;
    int icon_size;

    void set_mainwindow();

private:
    //void set_mainwindow();
};

#endif
