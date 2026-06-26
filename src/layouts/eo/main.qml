// SPDX-FileCopyrightText: 2026 Daniel O'Neill <daniel@oneill.app>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Components
import QtQuick.Layouts

KeyboardLayout {
    inputMode: InputEngine.InputMode.Latin
    keyWeight: 160
    readonly property real normalKeyWidth: normalKey.width
    readonly property real functionKeyWidth: mapFromItem(normalKey, normalKey.width / 2, 0).x
    KeyboardRow {
        Key {
            text: "ŝ"
            alternativeKeys: "q"
        }
        Key {
            id: normalKey
            text: "ĝ"
            alternativeKeys: "w"
        }
        Key {
            key: Qt.Key_E
            text: "e"
            alternativeKeys: "eèé"
        }
        Key {
            key: Qt.Key_R
            text: "r"
        }
        Key {
            key: Qt.Key_T
            text: "t"
        }
        Key {
            text: "ŭ"
            alternativeKeys: "y"
        }
        Key {
            key: Qt.Key_U
            text: "u"
            alternativeKeys: "uùú"
        }
        Key {
            key: Qt.Key_I
            text: "i"
            alternativeKeys: "iìí"
        }
        Key {
            key: Qt.Key_O
            text: "o"
            alternativeKeys: "oòó"
        }
        Key {
            key: Qt.Key_P
            text: "p"
        }
    }
    KeyboardRow {
        Key {
            key: Qt.Key_A
            text: "a"
            alternativeKeys: "aàá"
        }
        Key {
            key: Qt.Key_S
            text: "s"
        }
        Key {
            key: Qt.Key_D
            text: "d"
        }
        Key {
            key: Qt.Key_F
            text: "f"
        }
        Key {
            key: Qt.Key_G
            text: "g"
        }
        Key {
            key: Qt.Key_H
            text: "h"
            alternativeKeys: "ĥ"
        }
        Key {
            key: Qt.Key_J
            text: "j"
        }
        Key {
            key: Qt.Key_K
            text: "k"
        }
        Key {
            key: Qt.Key_L
            text: "l"
        }
        Key {
            text: "ĵ"
        }
    }
    KeyboardRow {
        ShiftKey {
            weight: functionKeyWidth
            Layout.fillWidth: false
        }
        Key {
            key: Qt.Key_Z
            text: "z"
        }
        Key {
            text: "ĉ"
            alternativeKeys: "x"
        }
        Key {
            key: Qt.Key_C
            text: "c"
        }
        Key {
            key: Qt.Key_V
            text: "v"
        }
        Key {
            key: Qt.Key_B
            text: "b"
        }
        Key {
            key: Qt.Key_N
            text: "n"
        }
        Key {
            key: Qt.Key_M
            text: "m"
        }
        BackspaceKey {
            weight: functionKeyWidth
            Layout.fillWidth: false
        }
    }
    KeyboardRow {
        SymbolModeKey {
            weight: functionKeyWidth
            Layout.fillWidth: false
        }
        ChangeLanguageKey {
            weight: normalKeyWidth
            Layout.fillWidth: false
        }
        Key {
            key: Qt.Key_Comma
            weight: normalKeyWidth
            Layout.fillWidth: false
            text: ","
            smallText: "\u2699"
            smallTextVisible: keyboard.isFunctionPopupListAvailable()
            highlighted: true
        }
        SpaceKey {
        }
        Key {
            key: Qt.Key_Period
            weight: normalKeyWidth
            Layout.fillWidth: false
            text: "."
            alternativeKeys: "!.?"
            smallText: "!?"
            smallTextVisible: true
            highlighted: true
        }
        HideKeyboardKey {
            weight: normalKeyWidth
            Layout.fillWidth: false
        }
        EnterKey {
            weight: functionKeyWidth
            Layout.fillWidth: false
        }
    }
}
