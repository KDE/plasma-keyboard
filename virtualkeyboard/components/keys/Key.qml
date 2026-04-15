// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard

// Standard key that sends text/key clicks through the input method.

AbstractKey {
    id: root

    /**
     * Logical key code sent to the input engine when the key is activated.
     */
    property int key: Qt.Key_unknown

    property string commitText: root.text
    property bool noKeyEvent: key === Qt.Key_unknown && commitText.length === 0
    readonly property bool alternativeKeysActive: {
        const popup = VirtualKeyboard.alternativeKeysPopup;
        return !!(popup && popup.active && popup.ownerKey === root);
    }

    function trigger() {
        if (!enabled) {
            return;
        }
        if (!noKeyEvent) {
            commitKeyText(root.commitText);
        }
        clicked();
    }

    function commitAlternativeText(text) {
        commitKeyText(text);
    }

    function commitKeyText(text) {
        const engine = VirtualKeyboard.inputEngine;
        if (engine) {
            engine.sendTextComposerKey(root.key, text);
        }
    }

    KeyMouseArea {
        keyItem: root
        repeatEnabled: root.repeat && !root.noKeyEvent
        pressAndHoldInterval: 500

        onRepeatTriggered: root.commitKeyText(root.commitText)

        onPressStarted: (mouse) => {
            const popup = VirtualKeyboard.alternativeKeysPopup;
            if (popup && popup.active && popup.ownerKey !== root) {
                popup.close();
            }
        }

        onPressAndHold: (mouse) => {
            const popup = VirtualKeyboard.alternativeKeysPopup;
            if (!popup || root.effectiveAlternativeKeys.length === 0) {
                return;
            }

            if (popup.openForKey(root)) {
                stopRepeat();
            }
        }

        onPositionChanged: (mouse) => {
            const popup = VirtualKeyboard.alternativeKeysPopup;
            if (pressed && popup && popup.active && popup.ownerKey === root) {
                popup.moveFromKey(root, mouse.x);
            }
        }

        onReleaseFinished: (mouse) => {
            const popup = VirtualKeyboard.alternativeKeysPopup;
            if (popup && popup.active && popup.ownerKey === root) {
                popup.commitCurrent();
                return;
            }

            if (root.noKeyEvent) {
                root.clicked();
                return;
            }

            if (!root.repeat) {
                root.commitKeyText(root.commitText);
            }
            root.clicked();
        }

        onCancelFinished: {
            const popup = VirtualKeyboard.alternativeKeysPopup;
            if (popup && popup.active && popup.ownerKey === root) {
                popup.close();
                return;
            }
        }
    }
}
