/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

import org.kde.plasma.keyboard

InputPanelWindow {
    id: root
    height: Screen.height
    width: Screen.width
    interactiveHeight: inputPanel.implicitHeight > 0 ? inputPanel.implicitHeight : 100
    color: 'transparent'

    InputListenerItem {
        id: thing
        focus: true
        engine: inputPanel.InputContext.inputEngine
    }

    InputPanel {
        id: inputPanel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        focusPolicy: Qt.NoFocus

        function updateLocales() {
            if (PlasmaKeyboardSettings.enabledLocales.length === 0) {
                // If there are no enabled locales, set it to the current locale
                // NOTE: If Qt.locale().name is not valid, then all keyboard layouts will be shown.
                let locale = Qt.locale().name;
                if (locale === "C") {
                    locale = "en_US";
                }
                VirtualKeyboardSettings.activeLocales = [locale];
            } else {
                VirtualKeyboardSettings.activeLocales = PlasmaKeyboardSettings.enabledLocales;
            }
        }

        Connections {
            target: VirtualKeyboardSettings
            function onAvailableLocalesChanged() {
                inputPanel.updateLocales();
            }
        }

        Connections {
            target: PlasmaKeyboardSettings
            function onEnabledLocalesChanged() {
                inputPanel.updateLocales();
            }
        }

        Component.onCompleted: {
            VirtualKeyboardSettings.styleName = "Breeze";
            inputPanel.updateLocales();
        }
    }
}
