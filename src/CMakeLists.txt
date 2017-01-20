set(SRC_LIST
    main.cpp)

add_executable(ukui-indicators ${SRC_LIST})

include_directories(
    ../indicator-application
    ../indicator-calendar
    ../indicator-desktop
    ${PROJECT_BINARY_DIR}
    ${UKUI_PANEL_APPLET_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS})

target_link_libraries(
    ukui-indicators
    indicator-application
    indicator-calendar
    indicator-desktop
    ${UKUI_PANEL_APPLET_LIBRARIES}
    ${GTK_LIBRARIES})

install(TARGETS ukui-indicators
        RUNTIME DESTINATION ${INSTALL_LIBEXEC_DIR})
