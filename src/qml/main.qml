/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.VirtualKeyboard

Window {
    width: Screen.width
    height: inputPanel.implicitHeight > 0 ? inputPanel.implicitHeight : 100
    color: Qt.transparent

    InputThing {
        id: thing
        focus: true
        engine: inputPanel.InputContext.inputEngine
    }

    InputPanel {
        id: inputPanel
        anchors.fill: parent
        focusPolicy: Qt.NoFocus
    }
}
