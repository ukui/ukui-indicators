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

    gulong change_size_handler_id;
    gulong change_background_handler_id;

    void set_mainwindow();

private:
    //void set_mainwindow();
};

#endif
