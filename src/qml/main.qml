/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.VirtualKeyboard

Window {
    readonly property QtObject engine: inputPanel.InputContext.inputEngine

    width: Screen.width
    height: inputPanel.implicitHeight > 0 ? inputPanel.implicitHeight : 100

    InputThing {
        id: thing
        focus: true
    }

    InputPanel {
        id: inputPanel
        anchors.fill: parent
        focusPolicy: Qt.NoFocus
    }
}
