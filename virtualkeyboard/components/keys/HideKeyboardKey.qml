// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard

// Action key to hide the keyboard.

ActionKey {
    id: root

    iconName: "input-keyboard-virtual-hide-symbolic"
    displayText: ""
    functionKey: true
    secondaryStyle: true

    onClicked: if (VirtualKeyboard.inputMethodConnection) VirtualKeyboard.inputMethodConnection.hide()
}
