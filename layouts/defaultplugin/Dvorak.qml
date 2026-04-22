// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    readonly property real functionKeyWeight: defaultKeyWeight * 1.5

    KeyboardRow {
        Key { key: Qt.Key_Apostrophe; text: "'" }
        Key { key: Qt.Key_Comma; text: "," }
        Key { key: Qt.Key_Period; text: "." }
        Key { key: Qt.Key_P; text: "p" }
        Key { key: Qt.Key_Y; text: "y" }
        Key { key: Qt.Key_F; text: "f" }
        Key { key: Qt.Key_G; text: "g" }
        Key { key: Qt.Key_C; text: "c" }
        Key { key: Qt.Key_R; text: "r" }
        Key { key: Qt.Key_L; text: "l" }
    }

    KeyboardRow {
        Key { key: Qt.Key_A; text: "a" }
        Key { key: Qt.Key_O; text: "o" }
        Key { key: Qt.Key_E; text: "e" }
        Key { key: Qt.Key_U; text: "u" }
        Key { key: Qt.Key_I; text: "i" }
        Key { key: Qt.Key_D; text: "d" }
        Key { key: Qt.Key_H; text: "h" }
        Key { key: Qt.Key_T; text: "t" }
        Key { key: Qt.Key_N; text: "n" }
        Key { key: Qt.Key_S; text: "s" }
    }

    KeyboardRow {
        ShiftKey { weight: functionKeyWeight }
        Key { key: Qt.Key_J; text: "j" }
        Key { key: Qt.Key_K; text: "k" }
        Key { key: Qt.Key_X; text: "x" }
        Key { key: Qt.Key_B; text: "b" }
        Key { key: Qt.Key_M; text: "m" }
        Key { key: Qt.Key_W; text: "w" }
        Key { key: Qt.Key_V; text: "v" }
        BackspaceKey { weight: functionKeyWeight }
    }

    KeyboardRow {
        SymbolModeKey { weight: functionKeyWeight }
        ChangeLanguageKey { weight: defaultKeyWeight }
        Key {
            key: Qt.Key_Q
            text: "q"
            weight: defaultKeyWeight
        }
        SpaceKey { weight: 300 }
        Key {
            key: Qt.Key_Z
            text: "z"
            weight: defaultKeyWeight
        }
        HideKeyboardKey { weight: defaultKeyWeight }
        EnterKey { weight: functionKeyWeight }
    }
}
