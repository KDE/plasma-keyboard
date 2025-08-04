add_executable(plasma-keyboard)

ecm_add_qtwayland_client_protocol(plasma-keyboard PROTOCOL ${WaylandProtocols_DATADIR}/unstable/input-method/input-method-unstable-v1.xml BASENAME input-method-unstable-v1)
ecm_add_qtwayland_client_protocol(plasma-keyboard PROTOCOL ${WaylandProtocols_DATADIR}/unstable/text-input/text-input-unstable-v1.xml BASENAME text-input-unstable-v1)
ecm_add_qtwayland_client_protocol(plasma-keyboard PROTOCOL "${Wayland_DATADIR}/wayland.xml" BASENAME wayland)

set_source_files_properties(dbus/org.sigxcpu.Feedback.Haptic.xml PROPERTIES INCLUDE vibrationevent.h)
qt_add_dbus_interfaces(dbusinterface_SRCS
    dbus/org.sigxcpu.Feedback.Haptic.xml)

target_sources(plasma-keyboard PRIVATE
    main.cpp
    inputlisteneritem.cpp
    inputmethod.cpp
    inputpanelwindow.cpp
    inputplugin.cpp
    qwaylandinputpanelshellintegration.cpp
    qwaylandinputpanelsurface.cpp
    vibration.cpp
    vibrationevent.h
    resources.qrc
    ${dbusinterface_SRCS}
)

ecm_add_qml_module(plasma-keyboard URI "org.kde.plasma.keyboard" GENERATE_PLUGIN_SOURCE)
ecm_target_qml_sources(plasma-keyboard SOURCES
    qml/main.qml
)
ecm_finalize_qml_module(plasma-keyboard)

kconfig_add_kcfg_files(plasma-keyboard GENERATE_MOC plasmakeyboardsettings.kcfgc)

target_link_libraries(plasma-keyboard
    PRIVATE
    KF6::CoreAddons
    KF6::I18n
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

install(DIRECTORY layouts DESTINATION ${CMAKE_INSTALL_PREFIX}/share/plasma/keyboard)
