/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

InputPanelWindow {
    id: root
    height: Screen.height
    width: Screen.width
    interactiveHeight: inputPanel.implicitHeight > 0 ? inputPanel.implicitHeight : 100
    color: 'transparent'

    InputThing {
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

        Component.onCompleted: {
            VirtualKeyboardSettings.styleName = "Breeze";
        }
    }
}
