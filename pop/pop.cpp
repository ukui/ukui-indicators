#include <string.h>
#include <cairo.h>
#include "pop.h"

#include <glib/gi18n.h>
#include <gio/gio.h>


class Pop;

static void button_clicked(GtkWidget *w, Pop *d);
static bool on_button_press(GtkWidget *w, GdkEventButton *event, Pop *d);
static void label_clicked(GtkLabel *label, GSettings *settings);
static void mainwindow_hide(GtkWidget *w);       


Pop::Pop(AppletData *ad) :
    applet(ad->applet),
    orientation(ad->orientation),
    icon_size(16)
{

    tray = na_tray_new_for_screen(gtk_widget_get_screen(GTK_WIDGET(applet)), orientation);
    //force_no_focus_padding(GTK_WIDGET(tray));

    set_mainwindow();

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(main_window), vbox);
    //gtk_box_pack_start(GTK_BOX(vbox), pop_menu, FALSE, FALSE, 0);

    GIcon *icon = g_themed_icon_new("forward");
    image = gtk_image_new_from_gicon(icon, GTK_ICON_SIZE_BUTTON);
    
    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button), image);
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);

    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;
    
    provider = gtk_css_provider_new ();
    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);                                                                                  
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider),    GTK_STYLE_PROVIDER_PRIORITY_USER);


    gtk_css_provider_load_from_data (provider,
                            "*{"
                            "color:none;"
                            " }"
                            " button {"                                   
                            "   background-color: transparent;"
                            "   background-image:none;"
                            "   border: none"
                            "}"
                            "{"
                            " button:hover {"
                            "   background-image: none;"
                            " border : none;"
                            "}" , -1, NULL);

    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref (provider);


    g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), this);
    g_signal_connect(button, "button-press-event", G_CALLBACK(on_button_press), this);
    
    
    event_box = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(event_box), FALSE);
    gtk_container_add(GTK_CONTAINER(event_box), button);
    gtk_widget_show_all(event_box);
   

}



void Pop::set_mainwindow()
{
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //gtk_window_set_type_hint(GTK_WINDOW(main_window), GDK_WINDOW_TYPE_HINT_POPUP_MENU);
    gtk_window_set_gravity(GTK_WINDOW(main_window), GDK_GRAVITY_SOUTH_EAST);
    gtk_container_set_border_width(GTK_CONTAINER(main_window), 5);
    gtk_window_set_decorated(GTK_WINDOW(main_window), FALSE);
    //gtk_window_set_skip_pager_hint(GTK_WINDOW(main_window), TRUE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(main_window), TRUE);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_MOUSE);
    gtk_widget_hide_on_delete(main_window);


    GSettings *settings;
	char *path,*applet_name,*applet_icon;
    
    GtkWidget *vbox = gtk_vbox_new(TRUE, 1);
    gtk_container_add(GTK_CONTAINER(main_window), vbox);

    for(int i = 1; i <= 20; i++) {
        path = g_strdup_printf ("%s%d/", "/org/ukui/panel/indicator/tray", i);
		settings = 		g_settings_new_with_path ("org.ukui.panel.indicator.tray", path);
		int number = g_settings_get_int(settings, "number");
        

	  	if(number != -1) {
			applet_name = 		g_settings_get_string (settings, "applet-name");
			applet_icon = 		g_settings_get_string (settings, "applet-icon");
			gboolean show_value = 	g_settings_get_boolean (settings, "show");
            if(g_strcmp0(applet_name, "ukui") == 0)
                break;
            GtkWidget *label = gtk_check_button_new_with_label(applet_name);
            g_signal_connect(label, "clicked", G_CALLBACK(label_clicked), settings);
            if(show_value) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(label), FALSE);
            } else {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(label), TRUE);
            }
            
            gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
        }        
    }
    g_signal_connect(main_window, "focus-out-event", G_CALLBACK(mainwindow_hide), NULL);

    //event_box = gtk_event_box_new();
    //gtk_event_box_set_visible_window(GTK_EVENT_BOX(event_box), FALSE);
    //gtk_container_add(GTK_CONTAINER(main_window), alignment);


    //gtk_container_add(GTK_CONTAINER(main_window), alignment);
    //gtk_widget_show_all(main_window);
}



Pop::~Pop()
{
    gtk_widget_destroy(main_window);
}

static void
button_clicked(GtkWidget *w, Pop *d)
{
    static bool show_now = true;
    GSettings *settings;
    char *path;
    
    if (gtk_widget_get_visible(d->main_window) == TRUE) {
        gtk_widget_hide(d->main_window);
    }
    
    if(show_now) {
        gtk_container_remove(GTK_CONTAINER(d->button), d->image);
        gtk_widget_destroy(d->image);
        
        GIcon *icon = g_themed_icon_new("back");
        d->image = gtk_image_new_from_gicon(icon, GTK_ICON_SIZE_BUTTON);
        
        gtk_container_add(GTK_CONTAINER(d->button), d->image);

        gtk_widget_show_all(d->button);

        for(int i = 1; i <= 20; i++) {
            path = g_strdup_printf ("%s%d/", "/org/ukui/panel/indicator/tray", i);
		    settings = 		g_settings_new_with_path ("org.ukui.panel.indicator.tray", path);
            g_settings_set_boolean(settings, "show", false);    
        }
        show_now = false;
    } else {
        gtk_container_remove(GTK_CONTAINER(d->button), d->image);
        gtk_widget_destroy(d->image);
        
        GIcon *icon = g_themed_icon_new("forward");
        d->image = gtk_image_new_from_gicon(icon, GTK_ICON_SIZE_BUTTON);
        
        gtk_container_add(GTK_CONTAINER(d->button), d->image);

        gtk_widget_show_all(d->button);
        
        for(int i = 1; i <= 20; i++) {
            path = g_strdup_printf ("%s%d/", "/org/ukui/panel/indicator/tray", i);
		    settings = 		g_settings_new_with_path ("org.ukui.panel.indicator.tray", path);
            g_settings_set_boolean(settings, "show", true);    
        }
        show_now = true;
    }
    
    //gtk_widget_show_all(d->main_window);
}

static bool
on_button_press(GtkWidget *w, GdkEventButton *event, Pop *d)
{
    if (event->button == 1)
        return FALSE;

    
    if (gtk_widget_get_visible(d->main_window) == FALSE) {
        gtk_widget_destroy(d->main_window);
        d->set_mainwindow();
        gtk_widget_show_all(d->main_window);
    } else {
        gtk_widget_hide(d->main_window);
    }
    return TRUE;
}

static void 
label_clicked(GtkLabel *label, GSettings *settings)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(label))) {
        g_settings_set_boolean(settings, "show", false);     
    } else {
        g_settings_set_boolean(settings, "show", true);
  }

    
  /*  gboolean show_value = 	g_settings_get_boolean (settings, "show");
    if(show_value) {
        g_settings_set_boolean(settings, "show", false);
        
    } else {
        g_settings_set_boolean(settings, "show", true);
    }*/
} 

static void mainwindow_hide(GtkWidget *w)
{
    gtk_widget_hide(w);
}


/*
applet_name = 		g_settings_get_string (settings, "applet-name");
			applet_icon = 		g_settings_get_string (settings, "applet-icon");
			gboolean show_value = 	g_settings_get_boolean (settings, "show");
			icon = 			gtk_icon_theme_load_icon (icon_theme,
				    		 		applet_icon,
				    		 		16,
				    		 		0,
				    		 		NULL);


*/