// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.plasma.keyboard.virtualkeyboard

// Action key that toggles shift and enables caps lock on double tap.

ActionKey {
    id: shiftKey

    /**
     * Whether this simulates a keyboard "shift click".
     * Desktop layouts use this so Shift+letter behaves like a real
     * keyboard shortcut/key event. Mobile layouts should leave this off.
     */
    property bool sendDirectKeyEvents: false

    property int __doubleTapIntervalMs: 250
    property double __lastTapTimestamp: 0

    iconName: {
        if (!VirtualKeyboard.inputEngine) {
            return "keyboard-caps-disabled-symbolic"
        }
        if (VirtualKeyboard.inputEngine.capsLockActive) {
            return "keyboard-caps-locked-symbolic"
        }
        if (VirtualKeyboard.inputEngine.shiftActive) {
            return "keyboard-caps-enabled-symbolic"
        }
        return "keyboard-caps-disabled-symbolic"
    }
    displayText: ""
    functionKey: true
    secondaryStyle: true
    highlighted: VirtualKeyboard.inputEngine && (VirtualKeyboard.inputEngine.shiftActive || VirtualKeyboard.inputEngine.capsLockActive)

    function syncDirectShift() {
        if (sendDirectKeyEvents && VirtualKeyboard.inputEngine) {
            VirtualKeyboard.inputEngine.sendDirectKey(Qt.Key_Shift, VirtualKeyboard.inputEngine.shiftActive);
        }
    }

    onClicked: {
        if (!VirtualKeyboard.inputEngine) {
            return;
        }

        const now = Date.now();

        if (VirtualKeyboard.inputEngine.capsLockActive) {
            VirtualKeyboard.inputEngine.capsLockActive = false;
            VirtualKeyboard.inputEngine.shiftActive = false;
            __lastTapTimestamp = 0;
            shiftKey.syncDirectShift();
            return;
        }

        if (VirtualKeyboard.inputEngine.shiftActive) {
            if (__lastTapTimestamp > 0 && (now - __lastTapTimestamp) <= __doubleTapIntervalMs) {
                VirtualKeyboard.inputEngine.capsLockActive = true;
                VirtualKeyboard.inputEngine.shiftActive = false;
            } else {
                VirtualKeyboard.inputEngine.shiftActive = false;
            }
            __lastTapTimestamp = 0;
        } else {
            VirtualKeyboard.inputEngine.shiftActive = true;
            __lastTapTimestamp = now;
        }
        shiftKey.syncDirectShift();
    }
}
