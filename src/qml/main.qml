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

    InputPanel {
        id: inputPanel
        width: root.width * (WindowMode.isFloating ? 0.5 : 1.0)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        focusPolicy: Qt.NoFocus

        Component.onCompleted: {
            VirtualKeyboardSettings.styleName = "Breeze";
        }
    }
}
