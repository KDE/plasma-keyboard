// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    textComposer: PinyinTextComposer { inputMode: "pinyin" }
    sharedLayouts: ["symbols"]
    readonly property real functionKeyWeight: defaultKeyWeight * 1.5

    KeyboardRow {
        Key { key: Qt.Key_Q; text: "q"; alternativeKeys: "1"; smallText: "1"}
        Key { key: Qt.Key_W; text: "w"; alternativeKeys: "2"; smallText: "2"}
        Key { key: Qt.Key_E; text: "e"; alternativeKeys: "3"; smallText: "3"}
        Key { key: Qt.Key_R; text: "r"; alternativeKeys: "4"; smallText: "4"}
        Key { key: Qt.Key_T; text: "t"; alternativeKeys: "5"; smallText: "5"}
        Key { key: Qt.Key_Y; text: "y"; alternativeKeys: "6"; smallText: "6"}
        Key { key: Qt.Key_U; text: "u"; alternativeKeys: "7"; smallText: "7"}
        Key { key: Qt.Key_I; text: "i"; alternativeKeys: "8"; smallText: "8"}
        Key { key: Qt.Key_O; text: "o"; alternativeKeys: "9"; smallText: "9"}
        Key { key: Qt.Key_P; text: "p"; alternativeKeys: "0"; smallText: "0"}
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
        Key {
            weight: functionKeyWeight
            enabled: inputEngine && inputEngine.preeditText.length > 0
            key: Qt.Key_Apostrophe
            text: "'"
        }
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
        Key { key: Qt.Key_Comma; weight: defaultKeyWeight; text: "\uFF0C"; secondaryStyle: true }
        SpaceKey { weight: 300 }
        Key { key: Qt.Key_Period; weight: defaultKeyWeight; text: "\u3002"; alternativeKeys: "\uFF1B\u3001\u3002\uFF1A\uFF0E？！"; smallText: "!?"; secondaryStyle: true }
        HideKeyboardKey { weight: defaultKeyWeight }
        EnterKey { weight: functionKeyWeight }
    }
}
