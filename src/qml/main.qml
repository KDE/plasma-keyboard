/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

import org.kde.plasma.keyboard
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

        // Whether the panel takes the full width of the screen
        readonly property bool isFullScreenWidth: PlasmaKeyboardSettings.panelScreenWidthPct === 1.0

        color: inputPanel.keyboard.style.theme.keyboardBackgroundColor

        // Provide shadow and radius when the keyboard is detached from edges
        corners {
            // When the window isn't floating, it's attached to the bottom of the screen
            bottomLeftRadius: (isFullScreenWidth || !WindowMode.isFloating) ? 0 : Kirigami.Units.cornerRadius
            bottomRightRadius: (isFullScreenWidth || !WindowMode.isFloating) ? 0 : Kirigami.Units.cornerRadius
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
        readonly property real topPadding: padding + dragHandleArea.height

        // Never let width & height to be 0, otherwise it can cause problems for setting interactiveRegion
        width: inputPanel.width > 0 ? (inputPanel.width + padding * 2) : 100
        height: inputPanel.height > 0 ? (inputPanel.height + dragHandleArea.height + padding * 2) : 100

        // Top drag area (in floating mode)
        Item {
            id: dragHandleArea
            visible: WindowMode.isFloating
            height: (WindowMode.isFloating ? Kirigami.Units.gridUnit : 0) + parent.padding
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            // Handle window dragging
            DragHandler {
                target: panelWrapper
                cursorShape: active ? Qt.DragMoveCursor : Qt.OpenHandCursor
            }

            // Drag handle
            Rectangle {
                anchors.centerIn: parent
                radius: height / 2
                height: 2
                width: Kirigami.Units.gridUnit * 4
                color: inputPanel.keyboard.style.theme.textOnPrimaryColor
            }
        }

        // Input panel
        InputPanel {
            id: inputPanel
            anchors {
                top: dragHandleArea.bottom
                left: parent.left
                leftMargin: parent.padding
            }

            // height is calculated by InputPanel
            width: root.width * PlasmaKeyboardSettings.panelScreenWidthPct

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
}
