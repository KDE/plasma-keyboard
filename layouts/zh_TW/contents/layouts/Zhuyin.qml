// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    textComposer: ZhuyinTextComposer {}
    sharedLayouts: ["symbols"]

    readonly property real functionKeyWeight: defaultKeyWeight * 1.5

    KeyboardRow {
        Key { text: "ㄅ"; alternativeKeys: "ㄅ1"; smallText: "1" }
        Key { text: "ㄉ"; alternativeKeys: "ㄉ2"; smallText: "2" }
        Key { text: "ˇ"; alternativeKeys: "ˇ3"; smallText: "3" }
        Key { text: "ˋ"; alternativeKeys: "ˋ4"; smallText: "4" }
        Key { text: "ㄓ"; alternativeKeys: "ㄓ5"; smallText: "5" }
        Key { text: "ˊ"; alternativeKeys: "ˊ6"; smallText: "6" }
        Key { text: "˙"; alternativeKeys: "˙7"; smallText: "7" }
        Key { text: "ㄚ"; alternativeKeys: "ㄚ8"; smallText: "8" }
        Key { text: "ㄞ"; alternativeKeys: "ㄞ9"; smallText: "9" }
        Key { text: "ㄢ"; alternativeKeys: "ㄢ0"; smallText: "0" }
        Key { text: "ㄦ"; alternativeKeys: "ㄦ-"; smallText: "-" }
    }

    KeyboardRow {
        Key { text: "ㄆ"; alternativeKeys: "ㄆqQ"; smallText: "q" }
        Key { text: "ㄊ"; alternativeKeys: "ㄊwW"; smallText: "w" }
        Key { text: "ㄍ"; alternativeKeys: "ㄍeE"; smallText: "e" }
        Key { text: "ㄐ"; alternativeKeys: "ㄐrR"; smallText: "r" }
        Key { text: "ㄔ"; alternativeKeys: "ㄔtT"; smallText: "t" }
        Key { text: "ㄗ"; alternativeKeys: "ㄗyY"; smallText: "y" }
        Key { text: "ㄧ"; alternativeKeys: "ㄧuU"; smallText: "u" }
        Key { text: "ㄛ"; alternativeKeys: "ㄛiI"; smallText: "i" }
        Key { text: "ㄟ"; alternativeKeys: "ㄟoO"; smallText: "o" }
        Key { text: "ㄣ"; alternativeKeys: "ㄣpP"; smallText: "p" }
    }

    KeyboardRow {
        Key { text: "ㄇ"; alternativeKeys: "ㄇaA"; smallText: "a" }
        Key { text: "ㄋ"; alternativeKeys: "ㄋsS"; smallText: "s" }
        Key { text: "ㄎ"; alternativeKeys: "ㄎdD"; smallText: "d" }
        Key { text: "ㄑ"; alternativeKeys: "ㄑfF"; smallText: "f" }
        Key { text: "ㄕ"; alternativeKeys: "ㄕgG"; smallText: "g" }
        Key { text: "ㄘ"; alternativeKeys: "ㄘhH"; smallText: "h" }
        Key { text: "ㄨ"; alternativeKeys: "ㄨjJ"; smallText: "j" }
        Key { text: "ㄜ"; alternativeKeys: "ㄜkK"; smallText: "k" }
        Key { text: "ㄠ"; alternativeKeys: "ㄠlL"; smallText: "l" }
        Key { text: "ㄤ"; alternativeKeys: "ㄤ…"; smallText: "…" }
    }

    KeyboardRow {
        Key { text: "ㄈ"; alternativeKeys: "ㄈzZ"; smallText: "z" }
        Key { text: "ㄌ"; alternativeKeys: "ㄌxX"; smallText: "x" }
        Key { text: "ㄏ"; alternativeKeys: "ㄏcC"; smallText: "c" }
        Key { text: "ㄒ"; alternativeKeys: "ㄒvV"; smallText: "v" }
        Key { text: "ㄖ"; alternativeKeys: "ㄖbB"; smallText: "b" }
        Key { text: "ㄙ"; alternativeKeys: "ㄙnN"; smallText: "n" }
        Key { text: "ㄩ"; alternativeKeys: "ㄩmM"; smallText: "m" }
        Key { text: "ㄝ"; alternativeKeys: "、ㄝ，"; smallText: "，" }
        Key { text: "ㄡ"; alternativeKeys: "。ㄡ．"; smallText: "．" }
        Key { text: "ㄥ"; alternativeKeys: "；：ㄥ"; smallText: "；" }
    }

    KeyboardRow {
        SymbolModeKey {
            weight: functionKeyWeight
        }
        ChangeLanguageKey {
            weight: defaultKeyWeight
        }
        Key {
            key: Qt.Key_Comma
            weight: defaultKeyWeight
            text: "，"
            secondaryStyle: true
        }
        SpaceKey { weight: 200 }
        Key {
            key: Qt.Key_Period
            weight: defaultKeyWeight
            text: "。"
            alternativeKeys: "；、。：．？！"
            smallText: "!?"
            secondaryStyle: true
        }
        BackspaceKey {
            weight: defaultKeyWeight
        }
        HideKeyboardKey {
            weight: defaultKeyWeight
        }
        EnterKey {
            weight: functionKeyWeight
        }
    }
}
