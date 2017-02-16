set(SRC_LIST
    indicator-desktop.cpp indicator-desktop.h)

add_library(indicator-desktop STATIC ${SRC_LIST})

pkg_check_modules(WNCK REQUIRED libwnck-3.0)

add_definitions(-DPIXMAPS_DIR="${PACKAGE_DATA_DIR}/indicator-desktop")

include_directories(
    ${PROJECT_BINARY_DIR}
    ${WNCK_INCLUDE_DIRS}
    ${MATE_PANEL_APPLET_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS})

target_link_libraries(
    indicator-desktop
    ${WNCK_LIBRARIES}
    ${MATE_PANEL_APPLET_LIBRARIES}
    ${GTK_LIBRARIES})

install(FILES show-desktop-normal.png show-desktop-hovered.png show-desktop-pressed.png
        DESTINATION "${PACKAGE_DATA_DIR}/indicator-desktop/"
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
