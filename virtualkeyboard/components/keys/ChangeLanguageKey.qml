// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard

// Emoji key that opens the keyboard layout/language selector when held.

AbstractKey {
    id: root

    property bool __held: false

    iconName: "smiley"
    smallIconName: "translate"
    displayText: ""
    functionKey: true
    secondaryStyle: true

    onClicked: {
        if (VirtualKeyboard.keyboardController) {
            VirtualKeyboard.keyboardController.symbolMode = false
        }
        if (VirtualKeyboard.emojiController) {
            VirtualKeyboard.emojiController.open()
        }
    }

    KeyMouseArea {
        keyItem: root
        pressAndHoldInterval: 500

        onPressStarted: root.__held = false

        onPressAndHold: {
            if (VirtualKeyboard.languagePopup) {
                root.__held = true
                VirtualKeyboard.languagePopup.showForItem(root)
            }
        }

        onReleaseFinished: {
            if (!root.__held) {
                root.trigger()
            }
            root.__held = false
        }

        onCancelFinished: root.__held = false
    }
}
