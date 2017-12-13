project(ukui-indicators)

cmake_minimum_required(VERSION 2.8.11)

#string(TIMESTAMP PACKAGE_VERSION "%Y%m%d")

include(GNUInstallDirs)
include(FindPkgConfig)

set(INSTALL_LIBEXEC_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/${CMAKE_INSTALL_LIBEXECDIR})
set(PACKAGE_DATA_DIR "/usr/share/${PROJECT_NAME}")
set(GSETTINGS_SCHEMAS_DIR "/usr/share/glib-2.0/schemas")
set(DBUS_SERVICE_DIR "/usr/share/dbus-1/services")
set(UKUI_PANEL_DATA_DIR "/usr/share/ukui-panel")
set(UKUI_PANEL_APPLETS_DIR ${UKUI_PANEL_DATA_DIR}/applets)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DNDEBUG)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -Wextra -fPIC -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-unused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wextra -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-variable")

pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(UKUI_PANEL_APPLET REQUIRED libukuipanelapplet-4.0)

add_definitions(-DGETTEXT_PACKAGE=${PROJECT_NAME})
add_definitions(-DPACKAGE_DATA_DIR="${PACKAGE_DATA_DIR}")

add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(indicator-application)
add_subdirectory(indicator-calendar)
add_subdirectory(indicator-desktop)

configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)

