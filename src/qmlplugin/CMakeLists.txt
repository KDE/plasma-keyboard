# SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

ecm_add_qml_module(keyboardlib URI org.kde.plasma.keyboard.lib GENERATE_PLUGIN_SOURCE DEPENDENCIES QtQuick)

set_source_files_properties(
    BreezeConstants.qml
    PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

ecm_target_qml_sources(keyboardlib SOURCES
    BreezeKeyPanel.qml
    BreezeConstants.qml
    BreezePopup.qml
)

target_link_libraries(keyboardlib
    PUBLIC
        Qt::Core
    PRIVATE
        Qt::Qml
        Qt::Gui
        Qt::Quick
)

ecm_finalize_qml_module(keyboardlib)
