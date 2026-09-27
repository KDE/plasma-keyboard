// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard

// Emoji key that opens the emoji view, and opens the keyboard layout/language selector when held.

AbstractKey {
    id: root

    iconName: "smiley"
    displayText: ""
    functionKey: true
    secondaryStyle: true

    // Used by KeyboardNavigation to call pressAndHold() when Enter is held on the key
    readonly property bool customPressAndHold: !!VirtualKeyboard.languagePopup
    signal pressAndHold()

    onClicked: {
        if (VirtualKeyboard.keyboardController) {
            VirtualKeyboard.keyboardController.symbolMode = false
        }
        VirtualKeyboard.emojiController?.open()
    }

    onPressAndHold: VirtualKeyboard.languagePopup?.showForItem(root)

    KeyMouseArea {
        // Whether the language selector was opened by holding the key
        property bool held: false

        keyItem: root
        pressAndHoldInterval: 500

        onPressStarted: held = false
        onPressAndHold: {
            if (root.customPressAndHold) {
                held = true
                root.pressAndHold()
            }
        }
        onReleaseFinished: {
            if (!held) {
                root.trigger()
            }
        }
    }
}
