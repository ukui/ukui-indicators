set(SRC_LIST
    pop.cpp pop.h
    na-tray.c na-tray.h
    na-tray-manager.c na-tray-manager.h
    na-tray-child.c na-tray-child.h
    fixedtip.c fixedtip.h
    na-marshal.c na-marshal.h)

add_library(pop STATIC ${SRC_LIST})

include_directories(
    ${PROJECT_BINARY_DIR}
    ${UKUI_PANEL_APPLET_INCLUDE_DIRS}
    ${DBUSMENU_GTK_INCLUDE_DIRS}
    ${DBUS_GLIB_INCLUDE_DIRS}
    ${GTKMM_INCLUDE_DIRS})

target_link_libraries(
    pop
    ${UKUI_PANEL_APPLET_LIBRARIES}
    X11)

