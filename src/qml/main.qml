/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.lib as PlasmaKeyboard

import org.kde.kirigami as Kirigami

InputPanelWindow {
    id: root
    height: Screen.height
    width: Screen.width
    color: 'transparent'

    interactiveRegion: Qt.rect(
        panelWrapper.x,
        panelWrapper.y,
        panelWrapper.width,
        panelWrapper.height
    )

    onVisibleChanged: {
        if (!visible) {
            // Reset keyboard navigation when hidden
            // Note: keyboard property is internal Qt API
            if (inputPanel.keyboard.navigationModeActive) {
                inputPanel.keyboard.navigationModeActive = false;
            }

            // Close language dialog
            languageDialog.close();
        }
    }

    InputListenerItem {
        id: thing
        focus: true
        engine: inputPanel.InputContext.inputEngine

        keyboardNavigationActive: inputPanel.keyboard.navigationModeActive

        onKeyNavigationPressed: (key) => {
            // HACK: invoke the Qt VirtualKeyboard keyboard navigation feature ourselves
            // See https://github.com/qt/qtvirtualkeyboard/blob/6d810ac41df96f1ad984f56e17f16860bec2abbf/src/virtualkeyboard/qvirtualkeyboardinputcontext_p.h#L110
            inputPanel.InputContext.priv.navigationKeyPressed(key, false);
        }
        onKeyNavigationReleased: (key) => {
            // HACK: invoke the Qt VirtualKeyboard keyboard navigation feature ourselves
            inputPanel.InputContext.priv.navigationKeyReleased(key, false);
        }
    }

    Kirigami.ShadowedRectangle {
        id: panelWrapper

        LanguagePopup {
            id: languageDialog
            style: inputPanel.keyboard.style
            keyboardPanel: inputPanel

            onShowSettings: root.showSettings()
        }

        // Whether the panel takes the full width of the screen
        readonly property bool isFullScreenWidth: PlasmaKeyboardSettings.panelFillScreenWidth

        color: PlasmaKeyboard.BreezeConstants.keyboardBackgroundColor

        // Provide shadow and radius when the keyboard is detached from edges
        corners {
            // The window isn't floating, so only curve the top
            bottomLeftRadius: Kirigami.Units.cornerRadius
            bottomRightRadius: Kirigami.Units.cornerRadius
            topLeftRadius: isFullScreenWidth ? 0 : Kirigami.Units.cornerRadius
            topRightRadius: isFullScreenWidth ? 0 : Kirigami.Units.cornerRadius
        }
        shadow {
            size: isFullScreenWidth ? 0 : 16
            color: Qt.rgba(0, 0, 0, 0.3)
        }

        // Starting x and y centers the panel on the bottom
        x: (root.width / 2) - (width / 2)
        y: root.height - height

        // Padding for background corners and panel drag area
        readonly property real padding: isFullScreenWidth ? 0 : Kirigami.Units.largeSpacing

        // Never let width & height to be 0, otherwise it can cause problems for setting interactiveRegion
        width: inputPanel.width > 0 ? (inputPanel.width + padding * 2) : 100
        height: inputPanel.height > 0 ? (inputPanel.height + padding * 2) : 100

        InputPanel {
            id: inputPanel
            anchors {
                top: parent.top
                topMargin: parent.padding
                left: parent.left
                leftMargin: parent.padding
            }

            // height is calculated by InputPanel
            width: inputPanel.keyboard.style ? inputPanel.keyboard.style.aspectRatio * inputPanel.keyboard.style.targetKeyboardHeight : 0

            focusPolicy: Qt.NoFocus
            externalLanguageSwitchEnabled: true
            onExternalLanguageSwitch: (localeList, currentIndex) => {
                languageDialog.show(inputPanel.keyboard.activeKey, localeList, currentIndex)
            }

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
}
