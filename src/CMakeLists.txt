add_executable(qvk)

ecm_add_qtwayland_client_protocol(qvk PROTOCOL ${WaylandProtocols_DATADIR}/unstable/input-method/input-method-unstable-v1.xml BASENAME input-method-unstable-v1)
ecm_add_qtwayland_client_protocol(qvk PROTOCOL ${WaylandProtocols_DATADIR}/unstable/text-input/text-input-unstable-v1.xml BASENAME text-input-unstable-v1)
ecm_add_qtwayland_client_protocol(qvk PROTOCOL "${Wayland_DATADIR}/wayland.xml" BASENAME wayland)

target_sources(qvk PRIVATE main.cpp inputmethod.cpp inputplugin.cpp qwaylandinputpanelshellintegration.cpp qwaylandinputpanelsurface.cpp)

ecm_add_qml_module(qvk URI "org.kde.qvk" GENERATE_PLUGIN_SOURCE)
ecm_target_qml_sources(qvk SOURCES qml/main.qml)
ecm_finalize_qml_module(qvk)

target_link_libraries(qvk PRIVATE KF6::CoreAddons KF6::I18n Qt::Gui Qt::GuiPrivate Qt::VirtualKeyboard Qt::WaylandClient Qt::WaylandClientPrivate Wayland::Client)

install(TARGETS qvk ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
