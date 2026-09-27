// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard

// Emoji key that opens the emoji view.

AbstractKey {
    id: root

    iconName: "smiley"
    displayText: ""
    functionKey: true
    secondaryStyle: true

    onClicked: {
        if (VirtualKeyboard.keyboardController) {
            VirtualKeyboard.keyboardController.symbolMode = false
        }
        VirtualKeyboard.emojiController?.open()
    }

    KeyMouseArea {
        keyItem: root

        onReleaseFinished: root.trigger()
    }
}
