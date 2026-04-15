// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard

// Action key that cycles through input modes provided by a keyboard layout.

ActionKey {
    id: root

    /**
     * Input modes to cycle through (that the text composer supports).
     */
    property var modes: []

    /**
     * Text composer to set the mode of.
     */
    property var textComposer

    /**
     * Whether to show the next mode as the current text on the key, instead of
     * the currently active mode.
     */
    property bool showNextMode: false

    readonly property string __currentInputMode: textComposer ? textComposer.inputMode : ""
    readonly property var __modes: (modes || [])
        .map(function(mode) {
            return { value: mode, label: mode };
        })
        .filter(function(mode) {
            return mode.value.length > 0;
        })
    readonly property int __currentIndex: __modes.findIndex(function(mode) {
        return mode.value === __currentInputMode;
    })
    readonly property var __currentMode: __currentIndex >= 0 ? __modes[__currentIndex] : (__modes[0] ?? null)
    readonly property var __nextMode: __modes.length === 0 ? null : __modes[(__currentIndex + 1) % __modes.length]
    readonly property var __displayedMode: showNextMode ? __nextMode : __currentMode

    displayText: __displayedMode ? __displayedMode.label : ""
    functionKey: true
    secondaryStyle: true

    onClicked: {
        if (textComposer && __nextMode) {
            textComposer.inputMode = __nextMode.value;
        }
    }
}
