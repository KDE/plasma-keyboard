# SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

ecm_add_qml_module(plasma-keyboard-layouts-defaultplugin
    URI org.kde.plasma.keyboard.layouts.defaultplugin
    GENERATE_PLUGIN_SOURCE
    DEPENDENCIES
        QtQuick
        QtQuick.Layouts
        org.kde.plasma.keyboard
        org.kde.plasma.keyboard.virtualkeyboard
        org.kde.plasma.keyboard.virtualkeyboard.components
)

ecm_target_qml_sources(plasma-keyboard-layouts-defaultplugin SOURCES
    Dialpad.qml
    Digits.qml
    Desktop.qml
    Dvorak.qml
    FullDesktop.qml
    Numbers.qml
    Qwerty.qml
    Symbols.qml
)

target_link_libraries(plasma-keyboard-layouts-defaultplugin
    PRIVATE
        Qt::Qml
        Qt::Quick
)

ecm_finalize_qml_module(plasma-keyboard-layouts-defaultplugin)
