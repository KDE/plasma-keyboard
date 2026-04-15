// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components
import "shared" as Shared

KeyboardLayoutDefinition {
    textComposer: ZhuyinTextComposer {}

    symbols: Shared.Symbols {}

    KeyboardLayout {
        id: root

        // 4 normal rows plus the zhuyin/number row at 75% height
        readonly property real rowCount: 4.75
        readonly property int fullRowWeight: 1300
        readonly property bool latinMode: inputEngine.textComposer?.inputMode === "latin"

        // Make panel higher than normal to accommodate the extra row, which is 75% of a typical row height
        panelHeightFactor: rowCount / 4

        // Keep normal key slots square: 13 key width units across / 4.75 key height units tall
        maxWidthToHeightRatio: (fullRowWeight / defaultKeyWeight) / rowCount

        KeyboardRow {
            heightWeight: defaultRowHeightWeight * 0.75

            FillerKey { weight: root.defaultKeyWeight / 2 }
            Key { key: Qt.Key_1; text: (root.latinMode ? "1" : "ㄅ"); alternativeKeys: (root.latinMode ? "1!" : "ㄅ1"); smallText: (root.latinMode ? "!" : "1") }
            Key { key: Qt.Key_2; text: (root.latinMode ? "2" : "ㄉ"); alternativeKeys: (root.latinMode ? "2@" : "ㄉ2"); smallText: (root.latinMode ? "@" : "2") }
            Key { key: Qt.Key_3; text: (root.latinMode ? "3" : "ˇ"); alternativeKeys: (root.latinMode ? "3#" : "ˇ3"); smallText: (root.latinMode ? "#" : "3") }
            Key { key: Qt.Key_4; text: (root.latinMode ? "4" : "ˋ"); alternativeKeys: (root.latinMode ? "4$" : "ˋ4"); smallText: (root.latinMode ? "$" : "4") }
            Key { key: Qt.Key_5; text: (root.latinMode ? "5" : "ㄓ"); alternativeKeys: (root.latinMode ? "5%" : "ㄓ5"); smallText: (root.latinMode ? "%" : "5") }
            Key { key: Qt.Key_6; text: (root.latinMode ? "6" : "ˊ"); alternativeKeys: (root.latinMode ? "6^" : "ˊ6"); smallText: (root.latinMode ? "^" : "6") }
            Key { key: Qt.Key_7; text: (root.latinMode ? "7" : "˙"); alternativeKeys: (root.latinMode ? "7&" : "˙7"); smallText: (root.latinMode ? "&" : "7") }
            Key { key: Qt.Key_8; text: (root.latinMode ? "8" : "ㄚ"); alternativeKeys: (root.latinMode ? "8*" : "ㄚ8"); smallText: (root.latinMode ? "*" : "8") }
            Key { key: Qt.Key_9; text: (root.latinMode ? "9" : "ㄞ"); alternativeKeys: (root.latinMode ? "9(" : "ㄞ9"); smallText: (root.latinMode ? "(" : "9") }
            Key { key: Qt.Key_0; text: (root.latinMode ? "0" : "ㄢ"); alternativeKeys: (root.latinMode ? "0)" : "ㄢ0"); smallText: (root.latinMode ? ")" : "0") }
            Key { key: Qt.Key_Minus; text: (root.latinMode ? "-" : "ㄦ"); alternativeKeys: (root.latinMode ? "-_" : "ㄦ-"); smallText: (root.latinMode ? "_" : "-") }
            BackspaceKey { weight: root.defaultKeyWeight * 1.5 }
        }

        KeyboardRow {
            KeysymKey { key: Qt.Key_Escape; displayText: "Esc"; }
            Key { key: Qt.Key_Q; text: (root.latinMode ? "q" : "ㄆ") }
            Key { key: Qt.Key_W; text: (root.latinMode ? "w" : "ㄊ") }
            Key { key: Qt.Key_E; text: (root.latinMode ? "e" : "ㄍ"); alternativeKeys: (root.latinMode ? "êeëèé" : []) }
            Key { key: Qt.Key_R; text: (root.latinMode ? "r" : "ㄐ"); alternativeKeys: (root.latinMode ? "ŕrř" : []) }
            Key { key: Qt.Key_T; text: (root.latinMode ? "t" : "ㄔ"); alternativeKeys: (root.latinMode ? "ţtŧť" : []) }
            Key { key: Qt.Key_Y; text: (root.latinMode ? "y" : "ㄗ"); alternativeKeys: (root.latinMode ? "ÿyýŷ" : []) }
            Key { key: Qt.Key_U; text: (root.latinMode ? "u" : "ㄧ"); alternativeKeys: (root.latinMode ? "űūũûüuùú" : []) }
            Key { key: Qt.Key_I; text: (root.latinMode ? "i" : "ㄛ"); alternativeKeys: (root.latinMode ? "îïīĩiìí" : []) }
            Key { key: Qt.Key_O; text: (root.latinMode ? "o" : "ㄟ"); alternativeKeys: (root.latinMode ? "œøõôöòóo" : []) }
            Key { key: Qt.Key_P; text: (root.latinMode ? "p" : "ㄣ") }
            Key { key: Qt.Key_BracketLeft; text: (root.latinMode ? "[" : "，"); alternativeKeys: (root.latinMode ? "[{" : "，、「"); smallText: (root.latinMode ? "{" : "「") }
            Key { key: Qt.Key_BracketRight; text: (root.latinMode ? "]" : "。"); alternativeKeys: (root.latinMode ? "]}" : "。．」"); smallText: (root.latinMode ? "}" : "」") }
        }

        KeyboardRow {
            KeysymKey { weight: defaultKeyWeight * 1.5; key: Qt.Key_Tab; displayText: "Tab" }
            Key { key: Qt.Key_A; text: (root.latinMode ? "a" : "ㄇ"); alternativeKeys: (root.latinMode ? "aäåãâàá" : []) }
            Key { key: Qt.Key_S; text: (root.latinMode ? "s" : "ㄋ"); alternativeKeys: (root.latinMode ? "šsşś" : []) }
            Key { key: Qt.Key_D; text: (root.latinMode ? "d" : "ㄎ"); alternativeKeys: (root.latinMode ? "dđď" : []) }
            Key { key: Qt.Key_F; text: (root.latinMode ? "f" : "ㄑ") }
            Key { key: Qt.Key_G; text: (root.latinMode ? "g" : "ㄕ"); alternativeKeys: (root.latinMode ? "ġgģĝğ" : []) }
            Key { key: Qt.Key_H; text: (root.latinMode ? "h" : "ㄘ") }
            Key { key: Qt.Key_J; text: (root.latinMode ? "j" : "ㄨ") }
            Key { key: Qt.Key_K; text: (root.latinMode ? "k" : "ㄜ") }
            Key { key: Qt.Key_L; text: (root.latinMode ? "l" : "ㄠ"); alternativeKeys: (root.latinMode ? "ĺŀłļľl" : []) }
            Key { key: Qt.Key_Apostrophe; text: (root.latinMode ? "'" : "ㄤ"); alternativeKeys: (root.latinMode ? "\"" : "ㄤ\""); smallText: "\""}
            EnterKey { weight: root.defaultKeyWeight * 1.5 }
        }

        KeyboardRow {
            ShiftKey { weight: root.defaultKeyWeight * 2; sendDirectKeyEvents: true }
            Key { key: Qt.Key_Z; text: (root.latinMode ? "z" : "ㄈ"); alternativeKeys: (root.latinMode ? "zžż" : []) }
            Key { key: Qt.Key_X; text: (root.latinMode ? "x" : "ㄌ") }
            Key { key: Qt.Key_C; text: (root.latinMode ? "c" : "ㄏ"); alternativeKeys: (root.latinMode ? "çcċčć" : []) }
            Key { key: Qt.Key_V; text: (root.latinMode ? "v" : "ㄒ") }
            Key { key: Qt.Key_B; text: (root.latinMode ? "b" : "ㄖ") }
            Key { key: Qt.Key_N; text: (root.latinMode ? "n" : "ㄙ"); alternativeKeys: (root.latinMode ? "ņńnň" : []) }
            Key { key: Qt.Key_M; text: (root.latinMode ? "m" : "ㄩ") }
            Key { key: Qt.Key_Comma; text: (root.latinMode ? "," : "ㄝ"); alternativeKeys: (root.latinMode ? ",;" : "ㄝ；"); smallText: (root.latinMode ? ";" : "；") }
            Key { key: Qt.Key_Period; text: (root.latinMode ? "." : "ㄡ"); alternativeKeys: (root.latinMode ? ".:" : "ㄡ："); smallText: (root.latinMode ? ":" : "：") }
            Key { key: Qt.Key_Slash; text: (root.latinMode ? "/" : "ㄥ"); alternativeKeys: (root.latinMode ? "/?!" : "ㄥ？！"); smallText: (root.latinMode ? "?" : "？") }
            ShiftKey { sendDirectKeyEvents: true }
        }

        KeyboardRow {
            SymbolModeKey { }
            KeysymKey { key: Qt.Key_Control; displayText: "Ctrl"; sticky: true }
            KeysymKey { key: Qt.Key_Meta; iconName: "start-here-symbolic"; displayText: ""; sticky: true }
            KeysymKey { key: Qt.Key_Alt; displayText: "Alt"; sticky: true }
            InputModeKey {
                modes: [
                    { value: "zhuyin", label: "中" },
                    { value: "latin", label: "英" }
                ]
            }
            SpaceKey { weight: root.defaultKeyWeight * 4 }
            KeysymKey { key: Qt.Key_Left; iconName: "go-previous-symbolic"; displayText: ""; repeat: true }
            KeyboardColumn {
                weight: root.defaultKeyWeight
                KeysymKey { implicitHeight: -1; key: Qt.Key_Up; iconName: "go-up-symbolic"; displayText: ""; repeat: true }
                KeysymKey { implicitHeight: -1; key: Qt.Key_Down; iconName: "go-down-symbolic"; displayText: ""; repeat: true }
            }
            KeysymKey { key: Qt.Key_Right; iconName: "go-next-symbolic"; displayText: ""; repeat: true }
            ChangeLanguageKey { weight: root.defaultKeyWeight / 2 }
            HideKeyboardKey { weight: root.defaultKeyWeight / 2 }
        }
    }
}
