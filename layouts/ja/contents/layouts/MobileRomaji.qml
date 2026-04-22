// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    textComposer: AnthyTextComposer { inputMode: "hiragana" }
    sharedLayouts: ["symbols"]
    readonly property real functionKeyWeight: defaultKeyWeight * 1.5

    KeyboardRow {
        Key { key: Qt.Key_Q; text: "q" }
        Key { key: Qt.Key_W; text: "w" }
        Key { key: Qt.Key_E; text: "e" }
        Key { key: Qt.Key_R; text: "r" }
        Key { key: Qt.Key_T; text: "t" }
        Key { key: Qt.Key_Y; text: "y" }
        Key { key: Qt.Key_U; text: "u" }
        Key { key: Qt.Key_I; text: "i" }
        Key { key: Qt.Key_O; text: "o" }
        Key { key: Qt.Key_P; text: "p" }
    }

    KeyboardRow {
        FillerKey { weight: defaultKeyWeight / 2 }
        Key { key: Qt.Key_A; text: "a" }
        Key { key: Qt.Key_S; text: "s" }
        Key { key: Qt.Key_D; text: "d" }
        Key { key: Qt.Key_F; text: "f" }
        Key { key: Qt.Key_G; text: "g" }
        Key { key: Qt.Key_H; text: "h" }
        Key { key: Qt.Key_J; text: "j" }
        Key { key: Qt.Key_K; text: "k" }
        Key { key: Qt.Key_L; text: "l" }
        FillerKey { weight: defaultKeyWeight / 2 }
    }

    KeyboardRow {
        ShiftKey { weight: functionKeyWeight }
        Key { key: Qt.Key_Z; text: "z" }
        Key { key: Qt.Key_X; text: "x" }
        Key { key: Qt.Key_C; text: "c" }
        Key { key: Qt.Key_V; text: "v" }
        Key { key: Qt.Key_B; text: "b" }
        Key { key: Qt.Key_N; text: "n" }
        Key { key: Qt.Key_M; text: "m" }
        BackspaceKey { weight: functionKeyWeight }
    }

    KeyboardRow {
        SymbolModeKey { weight: functionKeyWeight }
        ChangeLanguageKey { weight: defaultKeyWeight }
        Key {
            key: Qt.Key_Comma
            weight: defaultKeyWeight
            text: "、"
            secondaryStyle: true
        }
        SpaceKey { weight: defaultKeyWeight * 3 }
        Key {
            key: Qt.Key_Period
            weight: defaultKeyWeight
            text: "。"
            alternativeKeys: "、？！,.?!"
            smallText: "!?";
            secondaryStyle: true
        }
        HideKeyboardKey { weight: defaultKeyWeight }
        EnterKey { weight: functionKeyWeight }
    }
}
