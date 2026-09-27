// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard

// Emoji key that opens the keyboard layout/language selector when held.

AbstractKey {
    id: root

    iconName: "smiley"
    smallIconName: "translate"
    displayText: ""
    functionKey: true
    secondaryStyle: true

    onClicked: {
        if (VirtualKeyboard.keyboardController) {
            VirtualKeyboard.keyboardController.symbolMode = false
        }
        VirtualKeyboard.emojiController?.open()
    }

    // Keyboard navigation opens the language selector directly instead of emoji
    // TODO: properly support keyboard navigation in emoji view
    function triggerByNavigation() {
        VirtualKeyboard.languagePopup?.showForItem(root)
    }

    KeyMouseArea {
        // Whether this press opened the language selector, so releasing it doesn't also open emoji
        property bool held: false

        keyItem: root
        pressAndHoldInterval: 500

        onPressStarted: held = false
        onPressAndHold: {
            held = !!VirtualKeyboard.languagePopup
            VirtualKeyboard.languagePopup?.showForItem(root)
        }
        onReleaseFinished: {
            if (!held) {
                root.trigger()
            }
        }
    }
}
