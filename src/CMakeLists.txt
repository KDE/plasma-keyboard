# SPDX-FileCopyrightText: 2024 Aleix Pol <aleixpol@kde.org>
# SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
# SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>
#
# SPDX-License-Identifier: BSD-2-Clause

configure_file(config-plasma-keyboard.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-plasma-keyboard.h)

add_executable(plasma-keyboard)

ecm_add_qtwayland_client_protocol(plasma-keyboard PROTOCOL ${WaylandProtocols_DATADIR}/unstable/input-method/input-method-unstable-v1.xml BASENAME input-method-unstable-v1)
ecm_add_qtwayland_client_protocol(plasma-keyboard PROTOCOL ${WaylandProtocols_DATADIR}/unstable/text-input/text-input-unstable-v1.xml BASENAME text-input-unstable-v1)
ecm_add_qtwayland_client_protocol(plasma-keyboard PROTOCOL "${Wayland_DATADIR}/wayland.xml" BASENAME wayland)

target_sources(plasma-keyboard PRIVATE
    main.cpp
    inputlisteneritem.cpp
    inputlisteneritem.h
    inputmethod.cpp
    inputmethod_p.h
    inputpanelwindow.cpp
    inputpanelwindow.h
    inputplugin.cpp
    inputplugin.h
    qwaylandinputpanelshellintegration.cpp
    qwaylandinputpanelshellintegration_p.h
    qwaylandinputpanelsurface.cpp
    qwaylandinputpanelsurface_p.h
)

if (PLASMA_KEYBOARD_VIBRATION_ENABLED)
    set_source_files_properties(dbus/org.sigxcpu.Feedback.Haptic.xml PROPERTIES INCLUDE vibrationevent.h)
    qt_add_dbus_interfaces(dbusinterface_SRCS
        dbus/org.sigxcpu.Feedback.Haptic.xml)

    target_sources(plasma-keyboard PRIVATE
        vibration.cpp
        vibration.h
        vibrationevent.h
        ${dbusinterface_SRCS}
    )
endif()

if (PLASMA_KEYBOARD_SOUND_ENABLED)
    target_sources(plasma-keyboard PRIVATE
        soundresources.qrc
    )
endif()

ecm_add_qml_module(plasma-keyboard
    URI "org.kde.plasma.keyboard"
    GENERATE_PLUGIN_SOURCE
    DEPENDENCIES
        QtQuick
)

ecm_target_qml_sources(plasma-keyboard SOURCES
    qml/main.qml
    qml/LanguagePopup.qml
    qml/LanguagePopupDelegate.qml
)
ecm_finalize_qml_module(plasma-keyboard)

kconfig_add_kcfg_files(plasma-keyboard GENERATE_MOC plasmakeyboardsettings.kcfgc)

target_link_libraries(plasma-keyboard
    PRIVATE
    KF6::CoreAddons
    KF6::I18n
    KF6::I18nQml
    KF6::ConfigCore
    KF6::ConfigGui
    Qt::DBus
    Qt::Gui
    Qt::GuiPrivate
    Qt::VirtualKeyboard
    Qt::WaylandClient
    Qt::WaylandClientPrivate
    Wayland::Client
)

install(TARGETS plasma-keyboard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS org.kde.plasma.keyboard.desktop DESTINATION ${KDE_INSTALL_APPDIR})

add_subdirectory(styles)
add_subdirectory(qmlplugin)

install(DIRECTORY layouts DESTINATION ${CMAKE_INSTALL_PREFIX}/share/plasma/keyboard)

install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.plasma.keyboard.metainfo.xml
  DESTINATION ${KDE_INSTALL_METAINFODIR}
)
