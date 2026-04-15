// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard

// Desktop key that sends a keysym (ex. modifier) rather than a typical textual key.

AbstractKey {
    id: root

    /**
     * Logical key code sent through KWin fake input when the key is activated.
     */
    property int key: Qt.Key_unknown

    /**
     * Whether this key toggles between pressed and released when tapped.
     */
    property bool sticky: false

    /**
     * Whether this key is currently pressed through the direct key backend.
     */
    readonly property bool keyPressed: VirtualKeyboard.inputEngine && VirtualKeyboard.inputEngine.pressedKeys.indexOf(root.key) !== -1

    functionKey: true
    secondaryStyle: true
    noModifier: true
    showPreview: false
    highlighted: keyPressed

    function sendPressed(pressed) {
        if (VirtualKeyboard.inputEngine) {
            VirtualKeyboard.inputEngine.sendDirectKey(root.key, pressed);
        }
    }

    function sendClick() {
        root.sendPressed(true);
        root.sendPressed(false);
    }

    function trigger() {
        if (!enabled) {
            return;
        }
        if (root.sticky) {
            root.sendPressed(!root.keyPressed);
        } else {
            root.sendClick();
        }
        clicked();
    }

    KeyMouseArea {
        keyItem: root
        repeatEnabled: root.repeat

        onRepeatTriggered: root.sendClick()

        onReleaseFinished: {
            if (!root.repeat) {
                if (root.sticky) {
                    root.sendPressed(!root.keyPressed);
                } else {
                    root.sendClick();
                }
            }
            root.clicked()
        }
    }
}
