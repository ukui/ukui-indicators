configure_file(org.mate.panel.MateIndicatorsApplet.mate-panel-applet
               ${CMAKE_CURRENT_BINARY_DIR}/org.mate.panel.MateIndicatorsApplet.mate-panel-applet)

configure_file(org.mate.panel.applet.MateIndicatorsAppletFactory.service
               ${CMAKE_CURRENT_BINARY_DIR}/org.mate.panel.applet.MateIndicatorsAppletFactory.service)

configure_file(reset-applet-position.desktop
               ${CMAKE_CURRENT_BINARY_DIR}/reset-applet-position.desktop)

install(FILES org.mate.panel.indicators.gschema.xml
        DESTINATION ${GSETTINGS_SCHEMAS_DIR}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.mate.panel.MateIndicatorsApplet.mate-panel-applet
        DESTINATION ${MATE_PANEL_APPLETS_DIR}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.mate.panel.applet.MateIndicatorsAppletFactory.service
        DESTINATION ${DBUS_SERVICE_DIR}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/reset_applet_position.py
        DESTINATION ${INSTALL_LIBEXEC_DIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/reset-applet-position.desktop
        DESTINATION "/etc/xdg/autostart/"
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
