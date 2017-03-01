/*
 *  Copyright (C) 2016, Tianjin KYLIN Information Technology Co., Ltd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include <gtk/gtk.h>
#include <mate-panel-applet.h>
#include <glib/gi18n.h>
#include <stdlib.h>
#include <webkit2/webkit2.h>
#include <webkitdom/webkitdom.h>

#include "indicator-calendar.h"

class IndicatorCalendar;

static gboolean disable_context_menu(WebKitWebView *web_view,
                                     GtkWidget *default_menu,
                                     WebKitHitTestResult *hit_test_result,
                                     gboolean triggered_with_keyboard,
                                     gpointer user_data);
static gboolean on_button_press(GtkWidget *button, GdkEventButton *event, IndicatorCalendar *d);
static gboolean update_time(IndicatorCalendar *d);
static void popup_config(GtkAction *action, gpointer user_data);
static void settings_changed(GSettings *settings, gchar *key, IndicatorCalendar *d);
static void doc_loaded(WebKitWebView *web_view, IndicatorCalendar *d);
static void applet_button_clicked(GtkWidget *w, IndicatorCalendar *d);
static void reposition(GtkWidget *widget, IndicatorCalendar *d);
GdkColor get_border_color(char *color_name);
static gboolean draw_border(GtkWidget *widget, GdkEventExpose *event, IndicatorCalendar *d);

bool ischinese = FALSE;

IndicatorCalendar::IndicatorCalendar(AppletData *ad) :
    applet(ad->applet),
    orientation(ad->orientation),
    has_cursor(FALSE),
    button_pressed(FALSE)
{
    settings = g_settings_new("org.mate.panel.indicator.calendar");
    panel_settings = g_settings_new_with_path("org.mate.panel.toplevel", "/org/mate/panel/toplevels/bottom/");
    use_24h_format = g_settings_get_boolean(settings, "use-24h-format");
    show_second = g_settings_get_boolean(settings, "show-second");
    gchar *text = g_settings_get_string(settings, "theme");
    theme.assign(text);
    g_free(text);
    char *lang = getenv("LANG");
    if (strncmp(lang, "zh_CN", 5) == 0)
	ischinese = TRUE;
    g_signal_connect(settings, "changed", G_CALLBACK(settings_changed), this);

    _setup_main_window();
    _setup_popup_menu();
    _parse_rc();

    applet_label = gtk_label_new(NULL);
    gtk_widget_set_name(applet_label, "ukui-time-label");
    gtk_label_set_use_markup(GTK_LABEL(applet_label), TRUE);
    gtk_label_set_justify(GTK_LABEL(applet_label), GTK_JUSTIFY_CENTER);

    applet_button = gtk_button_new();
    gtk_button_set_relief(GTK_BUTTON(applet_button), GTK_RELIEF_NONE);
    gtk_widget_set_name(applet_button, "ukui-time-button");
    g_signal_connect(applet_button, "clicked", G_CALLBACK(applet_button_clicked), this);
    g_signal_connect(applet_button, "button-press-event", G_CALLBACK(on_button_press), this);
    gtk_container_add(GTK_CONTAINER(applet_button), applet_label);

    event_box = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(event_box), FALSE);
    gtk_container_add(GTK_CONTAINER(event_box), applet_button);
    gtk_widget_show_all(event_box);

    time_source = g_timeout_add(1000, (GSourceFunc)update_time, this);
    update_time(this);
}

IndicatorCalendar::~IndicatorCalendar()
{
    gtk_widget_destroy(main_window);
    gtk_widget_destroy(menu);
    g_object_unref(settings);
    g_source_remove(time_source);
}

void IndicatorCalendar::_setup_popup_menu()
{
    menu = gtk_menu_new();
    GtkWidget *item;
    if (ischinese)
        item = gtk_menu_item_new_with_label("时间和日期设置");
    else
        item = gtk_menu_item_new_with_label("Time and date settings");
    gtk_widget_show(item);
    g_signal_connect(item, "activate", G_CALLBACK(popup_config), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    gtk_menu_set_screen(GTK_MENU(menu), gtk_widget_get_screen(GTK_WIDGET(applet)));
}

void IndicatorCalendar::_parse_rc()
{
    static bool first_time = true;

    if (first_time) {
        gtk_rc_parse(PACKAGE_DATA_DIR"/indicator-calendar/ukui-calendar.rc");
        first_time = false;
    }
}

void IndicatorCalendar::_setup_main_window()
{
    webview = webkit_web_view_new();
    g_signal_connect(webview, "context-menu", G_CALLBACK(disable_context_menu), NULL);
    g_signal_connect(webview, "show", G_CALLBACK(doc_loaded), this);

    std::string html_file_path(PACKAGE_DATA_DIR "/indicator-calendar/html/");
    GdkColor color = get_border_color("ukuimenu_color");
    char color_str[20] = { 0 };
    sprintf(color_str, "%d%d%d", color.red, color.green, color.blue);
    if (ischinese)
    {
        char file[30] = {0};
        sprintf(file, "ukui.html.%s", color_str);
        html_file_path += file;
    }
    else
    {
	char file[30] = {0};
        sprintf(file, "ukui-en.html.%s", color_str);
        html_file_path += file;
    }

    std::string html_file_uri("file://");
    html_file_uri += html_file_path;
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), html_file_uri.c_str());

    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(main_window), 5);
    gtk_window_set_decorated(GTK_WINDOW(main_window), FALSE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(main_window), TRUE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(main_window), TRUE);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_MOUSE);
    gtk_window_set_gravity(GTK_WINDOW(main_window), GDK_GRAVITY_SOUTH_EAST);
    gtk_widget_hide_on_delete(main_window);
    gtk_container_add(GTK_CONTAINER(main_window), webview);
    g_signal_connect(main_window, "focus-out-event", G_CALLBACK(gtk_widget_hide), NULL);
//    g_signal_connect(main_window, "show", G_CALLBACK(reposition), this);
    g_signal_connect_after(main_window, "draw", G_CALLBACK(draw_border), this);
}

static void
menu_pos_func(GtkMenu *menu, gint *x, gint *y, gboolean *push_in, IndicatorCalendar *d)
{
    GtkAllocation alloc;
    GtkRequisition req;
    GdkScreen *screen;
    gint menu_x = 0;
    gint menu_y = 0;
    gint pointer_x = 0;
    gint pointer_y = 0;

    screen = gtk_widget_get_screen(GTK_WIDGET(d->applet));
    gtk_widget_size_request(GTK_WIDGET(menu), &req);

    gdk_window_get_origin(gtk_widget_get_window(d->event_box), &menu_x, &menu_y);
    gtk_widget_get_pointer(d->event_box, &pointer_x, &pointer_y);

    gtk_widget_get_allocation(d->event_box, &alloc);

    if (gtk_widget_get_has_window(d->event_box) == FALSE) {
        menu_x += alloc.x;
        menu_y += alloc.y;
    }

    if (mate_panel_applet_get_orient(d->applet) == MATE_PANEL_APPLET_ORIENT_UP ||
        mate_panel_applet_get_orient(d->applet) == MATE_PANEL_APPLET_ORIENT_DOWN) {
//        menu_x += alloc.width - req.width;
//        if (pointer_x > 0 && pointer_x < alloc.width &&
//            pointer_x < alloc.width - req.width) {
//            menu_x -= MIN(alloc.width - pointer_x, alloc.width - req.width);
//        }

        menu_x = MIN(menu_x, gdk_screen_get_width(screen) - req.width);

        if (menu_y > gdk_screen_get_height(screen) / 2)
            menu_y -= req.height;
        else
            menu_y += alloc.height;
    } else {

    }

    *x = menu_x;
    *y = menu_y;
    *push_in = TRUE;
}

static gboolean
on_button_press(GtkWidget *button, GdkEventButton *event, IndicatorCalendar *d)
{
    if (event->button == 1) {
        return FALSE;
    }

    gtk_menu_popup(GTK_MENU(d->menu),
                   NULL,
                   NULL,
                   (GtkMenuPositionFunc)menu_pos_func,
                   d, event->button, event->time);

    return TRUE;
}

static void
applet_button_clicked(GtkWidget *w, IndicatorCalendar *d)
{
    //WebKitDOMElement *year_div;
    //WebKitDOMElement *month_div;
    //WebKitDOMDocument *doc;

    //doc = webkit_web_page_get_dom_document(WEBKIT_WEB_VIEW(d->webview));
    //year_div = webkit_dom_document_get_element_by_id(doc, "year_div");
    //month_div = webkit_dom_document_get_element_by_id(doc, "month_div");

    if (gtk_widget_get_visible(d->main_window) == FALSE) {
        gtk_widget_show_all(d->main_window);
        webkit_web_view_reload(WEBKIT_WEB_VIEW(d->webview));
    } else {
        //webkit_dom_element_set_class_name(year_div, "hidden_div");
        //webkit_dom_element_set_class_name(month_div, "hidden_div");
        gtk_widget_hide(d->main_window);
    }

    //g_object_unref(doc);
    //g_object_unref(year_div);
    //g_object_unref(month_div);
}

static void popup_config(GtkAction *action, gpointer user_data)
{
    system("ukui-control-center &");
}

static gboolean
disable_context_menu(WebKitWebView *web_view,
                GtkWidget *default_menu,
                WebKitHitTestResult *hit_test_result,
                gboolean triggered_with_keyboard,
                gpointer user_data)
{
    return TRUE;
}

static void settings_changed(GSettings *settings, gchar *key, IndicatorCalendar *d)
{
    if (std::string(key) == "use-24h-format") {
        d->use_24h_format = g_settings_get_boolean(settings, "use-24h-format");
    } else if (std::string(key) == "show-second") {
        d->show_second = g_settings_get_boolean(settings, "show-second");
    } else if (std::string(key) == "theme") {
        gchar *new_theme = g_settings_get_string(settings, "theme");
        d->theme.assign(new_theme);
        g_free(new_theme);

        std::string html_file_path(PACKAGE_DATA_DIR "/indicator-calendar/html/");

	GdkColor color = get_border_color("ukuimenu_color");
	char color_str[20] = { 0 };
	sprintf(color_str, "%d%d%d", color.red, color.green, color.blue);
	if (ischinese)
	{
		char file[30] = {0};
		sprintf(file, "ukui.html.%s", color_str);
		html_file_path += file;
	}
	else
	{
		char file[30] = {0};
		sprintf(file, "ukui-en.html.%s", color_str);
		html_file_path += file;
	}

	std::string html_file_uri("file:://");
        html_file_uri += html_file_path;

        gtk_widget_modify_bg(d->main_window, GTK_STATE_NORMAL, &color);
        webkit_web_view_load_uri(WEBKIT_WEB_VIEW(d->webview), html_file_uri.c_str());

	gtk_widget_set_name(d->applet_label, "ukui-time-label");
    }
}

gchar *update_label(IndicatorCalendar *d, char *weekday, gint year, gint month, gint day, gint hour)
{
    gchar *markup = NULL;
    if (ischinese)
    {
        if (d->use_24h_format == FALSE)
            if (hour > 12)
                markup = g_strdup_printf("<span>%s%s下午\n %04d/%02d/%02d </span>", d->time.c_str(), weekday, year, month, day);
            else
                markup = g_strdup_printf("<span>%s%s上午\n %04d/%02d/%02d </span>", d->time.c_str(), weekday, year, month, day);
        else
            markup = g_strdup_printf("<span>%s %s\n %04d/%02d/%02d </span>", d->time.c_str(), weekday, year, month, day);
    }
    else
    {
        if (d->use_24h_format == FALSE)
            if (hour > 12)
                markup = g_strdup_printf("<span>%sAM %s\n %04d/%02d/%02d </span>", d->time.c_str(), weekday, year, month, day);
            else
                markup = g_strdup_printf("<span>%sPM %s\n %04d/%02d/%02d </span>", d->time.c_str(), weekday, year, month, day);
        else
            markup = g_strdup_printf("<span>%s %s\n %04d/%02d/%02d </span>", d->time.c_str(), weekday, year, month, day);
    }
    return markup;
}

static gboolean
update_time(IndicatorCalendar *d)
{
    gint year, month, day, hour, minute, second, week;
    gchar *text;
    char datestr[50];
    GDateTime *datetime;
    gchar *markup = NULL;

    datetime = g_date_time_new_now_local();

    week = g_date_time_get_day_of_week(datetime);
    hour = g_date_time_get_hour(datetime);
    minute = g_date_time_get_minute(datetime);
    second = g_date_time_get_second(datetime);
    g_date_time_get_ymd(datetime, &year, &month, &day);

    if (week == 1)
    {
	if (ischinese)
	{
            sprintf(datestr, "%d年%d月%d日星期一", year, month, day);
	    markup = update_label(d, (char *)("周一"), year, month, day, hour);
	}
	else
	{
	    sprintf(datestr, "%s Mon  %04d/%02d/%02d", d->time.c_str(), year, month, day);
	    markup = update_label(d, (char *)"Mon", year, month, day, hour);
	}
    }
    if (week == 2)
    {
	if (ischinese)
	{
	    markup = update_label(d, (char *)("周二"), year, month, day, hour);
            sprintf(datestr, "%d年%d月%d日星期二", year, month, day);
	}
	else
	{
	    markup = update_label(d, (char *)("Tues"), year, month, day, hour);
	    sprintf(datestr, "%s Tues  %04d/%02d/%02d", d->time.c_str(), year, month, day);
	}
    }
    if (week == 3)
    {
	if (ischinese)
	{
            sprintf(datestr, "%d年%d月%d日星期三", year, month, day);
	    markup = update_label(d, (char *)("周三"), year, month, day, hour);
	}
	else
	{
	    markup = update_label(d, (char *)("Wed"), year, month, day, hour);
	    sprintf(datestr, "%s Wed  %04d/%02d/%02d", d->time.c_str(), year, month, day);
	}
    }
    if (week == 4)
    {
	if (ischinese)
	{
	    markup = update_label(d, (char *)("周四"), year, month, day, hour);
            sprintf(datestr, "%d年%d月%d日星期四", year, month, day);
	}
	else
	{
	    markup = update_label(d, (char *)("Thur"), year, month, day, hour);
	    sprintf(datestr, "%s Thur  %04d/%02d/%02d", d->time.c_str(), year, month, day);
	}
    }
    if (week == 5)
    {
	if (ischinese)
	{
	    markup = update_label(d, (char *)("周五"), year, month, day, hour);
            sprintf(datestr, "%d年%d月%d日星期五", year, month, day);
	}
	else
	{
	    markup = update_label(d, (char *)("Fri"), year, month, day, hour);
	    sprintf(datestr, "%s Fri  %04d/%02d/%02d", d->time.c_str(), year, month, day);
	}
    }
    if (week == 6)
    {
	if (ischinese)
	{
	    markup = update_label(d, (char *)("周六"), year, month, day, hour);
            sprintf(datestr, "%d年%d月%d日星期六", year, month, day);
	}
	else
	{
	    markup = update_label(d, (char *)("Sat"), year, month, day, hour);
	    sprintf(datestr, "%s Sat  %04d/%02d/%02d", d->time.c_str(), year, month, day);
	}
    }
    if (week == 7)
    {
	if (ischinese)
	{
	    markup = update_label(d, (char *)("周日"), year, month, day, hour);
            sprintf(datestr, "%d年%d月%d日星期天", year, month, day);
	}
	else
	{
	    markup = update_label(d, (char *)("Sun"), year, month, day, hour);
	    sprintf(datestr, "%s Sun  %04d/%02d/%02d", d->time.c_str(), year, month, day);
	}
    }

    gtk_widget_set_tooltip_text(d->applet_label, datestr);

    if (d->use_24h_format == FALSE)
        hour = hour > 12 ? hour - 12 : hour;

    if (d->show_second)
        text = g_strdup_printf("%02d:%02d:%02d", hour, minute, second);
    else
        text = g_strdup_printf("%02d:%02d", hour, minute);
    d->time.assign(text);
    g_free(text);

    PangoLayout *layout = gtk_widget_create_pango_layout (d->applet_label, d->time.c_str());
    int height;
    static const int padding = 2;
    pango_layout_get_pixel_size(layout, NULL, &height);
    if (mate_panel_applet_get_size(d->applet) > static_cast<guint>(2 * height + 2 * padding)) {
        gtk_label_set_markup(GTK_LABEL(d->applet_label), markup);
        g_free(markup);
    } else {
        markup = g_strdup_printf("<span> %02d月%02d日 %s </span>", month, day, d->time.c_str());
        gtk_label_set_markup(GTK_LABEL(d->applet_label), markup);
        g_free(markup);
    }

    g_object_unref (layout);

    // 过0点时刷新html页面
    if ((hour == 0 && minute == 0 && second == 1) || (hour == 12 && minute == 0 && second == 1))
        webkit_web_view_reload(WEBKIT_WEB_VIEW(d->webview));

    g_date_time_unref(datetime);

    return G_SOURCE_CONTINUE;
}

static void
doc_loaded(WebKitWebView *web_view, IndicatorCalendar *d)
{
/*    WebKitDOMDocument *doc;
    WebKitDOMElement *header, *calendar, *right_pane, *general_datetime_list, *hl_table;

    doc = webkit_web_view_get_dom_document(WEBKIT_WEB_VIEW(d->webview));

    header = webkit_dom_document_get_element_by_id(doc, "header");
    calendar = webkit_dom_document_get_element_by_id(doc, "calendar_table");
    right_pane = webkit_dom_document_get_element_by_id(doc, "right_pane");
    general_datetime_list = webkit_dom_document_get_element_by_id(doc, "general_datetime_list");
    hl_table = webkit_dom_document_get_element_by_id(doc, "hl_table");

    // TODO: read the margins instead of this magic 20
    gulong width = webkit_dom_element_get_offset_width(calendar) +
                   webkit_dom_element_get_offset_width(right_pane) +
                   25;
//    gulong height1 = webkit_dom_element_get_offset_height(header) +
//                    webkit_dom_element_get_offset_height(calendar);
    gulong height2 = webkit_dom_element_get_offset_height(header) +
                    webkit_dom_element_get_offset_height(general_datetime_list) +
                    webkit_dom_element_get_offset_height(hl_table) +
                    12; // FIXMED: 5?
*/
    if (ischinese)
        gtk_widget_set_size_request(d->main_window, 480, 400);
    else
        gtk_widget_set_size_request(d->main_window, 500, 280);


/*    g_object_unref(doc);
    g_object_unref(calendar);
    g_object_unref(right_pane);
    g_object_unref(general_datetime_list);
    g_object_unref(hl_table);*/
}

static void reposition(GtkWidget *widget, IndicatorCalendar *d)
{
    GtkAllocation widget_alloc;
    gtk_widget_get_allocation (widget, &widget_alloc);

    GdkScreen *screen = gtk_widget_get_screen(d->event_box);

    int origin_x, origin_y;
    gdk_window_get_origin(gtk_widget_get_window(d->event_box), &origin_x, &origin_y);

    GtkAllocation applet_alloc;
    gtk_widget_get_allocation(d->event_box, &applet_alloc);

    if (gtk_widget_get_has_window(d->event_box) == FALSE) {
        origin_x += applet_alloc.x;
        origin_y += applet_alloc.y;
    }

    int width = gdk_screen_get_width (screen);
    int height = gdk_screen_get_height (screen);

    int panel_size;
    if (d->panel_settings)
        panel_size = g_settings_get_int (d->panel_settings, "size");
    else
        panel_size = 40;

    GdkRectangle monitor_rect;
    gdk_screen_get_monitor_geometry (screen,
                                     gdk_screen_get_monitor_at_point (screen, origin_x, origin_y),
                                     &monitor_rect);

    static const int gap = 1;
    widget_alloc.width += gap;
    widget_alloc.height += gap;

    int x, y;
    if (d->orientation == GTK_ORIENTATION_HORIZONTAL) {
        if (origin_x + widget_alloc.width > monitor_rect.x + monitor_rect.width)
            x = monitor_rect.x + monitor_rect.width;
        else
            x = origin_x + widget_alloc.width;

        if (origin_y + widget_alloc.height > monitor_rect.y + monitor_rect.height)
            y = monitor_rect.y + monitor_rect.height - panel_size;
        else
            y = monitor_rect.y + panel_size + widget_alloc.height;
    } else {
        // TODO: vertical panel mode
    }

    gtk_window_move (GTK_WINDOW(widget), x, y);
}

GdkColor
get_border_color (char *color_name)
{
        GdkColor color;

        GObject *gs = (GObject *)gtk_settings_get_default ();
        GValue color_scheme_value = G_VALUE_INIT;
        g_value_init (&color_scheme_value, G_TYPE_STRING);
        g_object_get_property (gs, "gtk-color-scheme", &color_scheme_value);
        gchar *color_scheme = (char *)g_value_get_string (&color_scheme_value);
        gchar color_spec[16] = { 0 };
        char *needle = strstr(color_scheme, color_name);
        if (needle) {
                while (1) {
                        if (color_spec[0] != '#') {
                                color_spec[0] = *needle;
                                needle++;
                                continue;
                        }

                        if ((*needle >= 0x30 && *needle <= 0x39) ||
                            (*needle >= 0x41 && *needle <= 0x46) ||
                            (*needle >= 0x61 && *needle <= 0x66)) {
                                color_spec[strlen(color_spec)] = *needle;
                                needle++;
                        } else {
                                break;
                        }
                }
                gdk_color_parse (color_spec, &color);
        } else {
                gdk_color_parse ("#3B9DC5", &color);
        }

        return color;
}

/*static gboolean draw_border(GtkWidget *widget, GdkEventExpose *event, IndicatorCalendar *d)
{
    cairo_t *cr = gdk_cairo_create(widget->window);

    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);

    if (d->theme == "ukui")
        cairo_set_source_rgb(cr, 0x15 / 255.0, 0x56 / 255.0, 0x70 / 255.0);
    else
        cairo_set_source_rgb(cr, 0x99 / 255.0, 0x99 / 255.0, 0x99 / 255.0);
    cairo_rectangle(cr, 0.5, 0.5, alloc.width - 0.5, alloc.height - 0.5);
    cairo_stroke(cr);

    cairo_destroy(cr);
    return FALSE;
}
*/

static gboolean
draw_border (GtkWidget *widget, GdkEventExpose *event, IndicatorCalendar *d)
{
	/*window成员使用GTK3中新的获取方式*/
        cairo_t *cr = gdk_cairo_create (gtk_widget_get_window(widget));

        GtkAllocation alloc;
        gtk_widget_get_allocation(widget, &alloc);

        GdkColor color = get_border_color ("ukuiside_color");

        // outer border
        cairo_set_source_rgb (cr, color.red / 65535.0, color.green / 65535.0, color.blue / 65535.0);
        cairo_set_line_width (cr, 1.0);
        cairo_rectangle (cr, 0.5, 0.5, alloc.width - 1, alloc.height - 1);
        cairo_stroke (cr);

        color = get_border_color ("ukuimenu_color");
	cairo_set_source_rgb (cr, color.red / 65535.0, color.green / 65535.0, color.blue / 65535.0);
        cairo_set_line_width (cr, 4.0);
        cairo_rectangle (cr, 3, 3, alloc.width - 6, alloc.height - 6);
        cairo_stroke (cr);
//        gtk_widget_modify_bg (widget, GTK_STATE_NORMAL, &color);

	cairo_destroy(cr);
        return FALSE;
}

