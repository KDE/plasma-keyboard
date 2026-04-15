// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard

// Action key that opens the keyboard layout/language selector.

ActionKey {
    id: root

    iconName: "globe-symbolic"
    displayText: ""
    functionKey: true
    secondaryStyle: true

    onClicked: {
        if (VirtualKeyboard.languagePopup) {
            VirtualKeyboard.languagePopup.showForItem(root)
        }
    }
}
