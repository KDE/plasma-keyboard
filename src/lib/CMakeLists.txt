# SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

ecm_add_qml_module(keyboardlib URI org.kde.plasma.keyboard.lib GENERATE_PLUGIN_SOURCE DEPENDENCIES QtQuick)

ecm_target_qml_sources(keyboardlib SOURCES
    qml/BreezeKeyPanel.qml
    qml/BreezeTheme.qml
    qml/KeysymKey.qml)

target_link_libraries(keyboardlib
    PUBLIC
        Qt::Core
    PRIVATE
        Qt::Qml
        Qt::Gui
        Qt::Quick
)

ecm_finalize_qml_module(keyboardlib)
