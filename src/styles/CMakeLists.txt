# SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later

ecm_add_qml_module(breezestyle URI QtQuick.VirtualKeyboard.Styles.Breeze GENERATE_PLUGIN_SOURCE DEPENDENCIES QtQuick)

ecm_target_qml_sources(breezestyle SOURCES
    breeze/style.qml
)

target_link_libraries(breezestyle
    PUBLIC
        Qt::Core
    PRIVATE
        Qt::Qml
        Qt::Quick
)

ecm_finalize_qml_module(breezestyle)
