configure_file(org.ukui.panel.UkuiIndicatorsApplet.ukui-panel-applet
               ${CMAKE_CURRENT_BINARY_DIR}/org.ukui.panel.UkuiIndicatorsApplet.ukui-panel-applet)

configure_file(org.ukui.panel.applet.UkuiIndicatorsAppletFactory.service
               ${CMAKE_CURRENT_BINARY_DIR}/org.ukui.panel.applet.UkuiIndicatorsAppletFactory.service)

configure_file(reset-applet-position.desktop
               ${CMAKE_CURRENT_BINARY_DIR}/reset-applet-position.desktop)

install(FILES org.ukui.panel.indicators.gschema.xml
        DESTINATION ${GSETTINGS_SCHEMAS_DIR}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.ukui.panel.UkuiIndicatorsApplet.ukui-panel-applet
	DESTINATION ${UKUI_PANEL_APPLETS_DIR}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.ukui.panel.applet.UkuiIndicatorsAppletFactory.service
        DESTINATION ${DBUS_SERVICE_DIR}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/reset_applet_position.py
        DESTINATION ${INSTALL_LIBEXEC_DIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/reset-applet-position.desktop
        DESTINATION "/etc/xdg/autostart/"
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
