set(SRC_LIST
    indicator-calendar.cpp indicator-calendar.h)

add_library(indicator-calendar STATIC ${SRC_LIST})

pkg_check_modules(WEBKIT REQUIRED webkit2gtk-4.0)

include_directories(
    ${PROJECT_BINARY_DIR}
    ${MATE_PANEL_APPLET_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${WEBKIT_INCLUDE_DIRS})

target_link_libraries(
    indicator-calendar
    ${MATE_PANEL_APPLET_LIBRARIES}
    ${GTK_LIBRARIES}
    ${WEBKIT_LIBRARIES})

# note the trailing slash
install(DIRECTORY html/
        DESTINATION ${PACKAGE_DATA_DIR}/indicator-calendar/html)

install(FILES ukui-calendar.rc calendar-button-bg.png
        DESTINATION "${PACKAGE_DATA_DIR}/indicator-calendar")
