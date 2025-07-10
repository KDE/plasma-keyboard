/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

ListView {
    id: root

    model: []

    Connections {
        target: VirtualKeyboardSettings

        function onAvailableLocalesChanged() {
            root.model = VirtualKeyboardSettings.availableLocales;
        }
    }

    // HACK: needed to populate VirtualKeyboardSettings.availableLocales
    InputPanel {}

    delegate: QQC2.CheckDelegate {
        width: root.width
        text: Qt.locale(modelData).nativeLanguageName
        checked: kcm.enabledLocales.includes(modelData)
        onCheckedChanged: {
            if (checked) {
                kcm.enableLocale(modelData);
            } else {
                kcm.disableLocale(modelData);
            }
        }
    }
}
