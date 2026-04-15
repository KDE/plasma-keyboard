// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    readonly property int rowCount: 5
    readonly property int fullRowWeight: 1550
    readonly property bool fnActive: inputEngine && inputEngine.pressedKeys.indexOf(KeyCodes.KeyFn) !== -1
    textComposer: ZhuyinTextComposer { inputMode: "zhuyin" }
    sharedLayouts: ["symbols"]
    readonly property bool latinMode: textComposer.inputMode === "latin"

    // Make panel higher than normal to accomodate 5 rows instead of 4, and the top row is 75% of a typical row height
    panelHeightFactor: 1.1875

    // Keep normal key slots square: 15.5 key-width units across / 5 key-height units tall
    maxWidthToHeightRatio: (fullRowWeight / defaultKeyWeight) / rowCount

    KeyboardRow {
        heightWeight: defaultRowHeightWeight * 0.75

        KeysymKey { key: Qt.Key_Escape; displayText: "Esc" }
        Key { key: Qt.Key_QuoteLeft; text: "`"; alternativeKeys: "`~"; smallText: "~" }
        Key {
            key: root.fnActive ? Qt.Key_F1 : Qt.Key_1
            text: root.fnActive ? "" : (root.latinMode ? "1" : "ㄅ")
            displayText: root.fnActive ? "F1" : (root.latinMode ? "1" : "ㄅ")
            alternativeKeys: root.fnActive ? [] : (root.latinMode ? "1!" : "ㄅ1!")
            smallText: root.fnActive ? "" : (root.latinMode ? "!" : "1")
        }
        Key {
            key: root.fnActive ? Qt.Key_F2 : Qt.Key_2
            text: root.fnActive ? "" : (root.latinMode ? "2" : "ㄉ")
            displayText: root.fnActive ? "F2" : (root.latinMode ? "2" : "ㄉ")
            alternativeKeys: root.fnActive ? [] : (root.latinMode ? "2@" : "ㄉ2@")
            smallText: root.fnActive ? "" : (root.latinMode ? "@" : "2")
        }
        Key {
            key: root.fnActive ? Qt.Key_F3 : Qt.Key_3
            text: root.fnActive ? "" : (root.latinMode ? "3" : "ˇ")
            displayText: root.fnActive ? "F3" : (root.latinMode ? "3" : "ˇ")
            alternativeKeys: root.fnActive ? [] : (root.latinMode ? "3#" : "ˇ3#")
            smallText: root.fnActive ? "" : (root.latinMode ? "#" : "3")
        }
        Key {
            key: root.fnActive ? Qt.Key_F4 : Qt.Key_4
            text: root.fnActive ? "" : (root.latinMode ? "4" : "ˋ")
            displayText: root.fnActive ? "F4" : (root.latinMode ? "4" : "ˋ")
            alternativeKeys: root.fnActive ? [] : (root.latinMode ? "4$" : "ˋ4$")
            smallText: root.fnActive ? "" : (root.latinMode ? "$" : "4")
        }
        Key {
            key: root.fnActive ? Qt.Key_F5 : Qt.Key_5
            text: root.fnActive ? "" : (root.latinMode ? "5" : "ㄓ")
            displayText: root.fnActive ? "F5" : (root.latinMode ? "5" : "ㄓ")
            alternativeKeys: root.fnActive ? [] : (root.latinMode ? "5%" : "ㄓ5%")
            smallText: root.fnActive ? "" : (root.latinMode ? "%" : "5")
        }
        Key {
            key: root.fnActive ? Qt.Key_F6 : Qt.Key_6
            text: root.fnActive ? "" : (root.latinMode ? "6" : "ˊ")
            displayText: root.fnActive ? "F6" : (root.latinMode ? "6" : "ˊ")
            alternativeKeys: root.fnActive ? [] : (root.latinMode ? "6^" : "ˊ6^")
            smallText: root.fnActive ? "" : (root.latinMode ? "^" : "6")
        }
        Key {
            key: root.fnActive ? Qt.Key_F7 : Qt.Key_7
            text: root.fnActive ? "" : (root.latinMode ? "7" : "˙")
            displayText: root.fnActive ? "F7" : (root.latinMode ? "7" : "˙")
            alternativeKeys: root.fnActive ? [] : (root.latinMode ? "7&" : "˙7&")
            smallText: root.fnActive ? "" : (root.latinMode ? "&" : "7")
        }
        Key {
            key: root.fnActive ? Qt.Key_F8 : Qt.Key_8
            text: root.fnActive ? "" : (root.latinMode ? "8" : "ㄚ")
            displayText: root.fnActive ? "F8" : (root.latinMode ? "8" : "ㄚ")
            alternativeKeys: root.fnActive ? [] : (root.latinMode ? "8*" : "ㄚ8*")
            smallText: root.fnActive ? "" : (root.latinMode ? "*" : "8")
        }
        Key {
            key: root.fnActive ? Qt.Key_F9 : Qt.Key_9
            text: root.fnActive ? "" : (root.latinMode ? "9" : "ㄞ")
            displayText: root.fnActive ? "F9" : (root.latinMode ? "9" : "ㄞ")
            alternativeKeys: root.fnActive ? [] : (root.latinMode ? "9(" : "ㄞ9(")
            smallText: root.fnActive ? "" : (root.latinMode ? "(" : "9")
        }
        Key {
            key: root.fnActive ? Qt.Key_F10 : Qt.Key_0
            text: root.fnActive ? "" : (root.latinMode ? "0" : "ㄢ")
            displayText: root.fnActive ? "F10" : (root.latinMode ? "0" : "ㄢ")
            alternativeKeys: root.fnActive ? [] : (root.latinMode ? "0)" : "ㄢ0)")
            smallText: root.fnActive ? "" : (root.latinMode ? ")" : "0")
        }
        Key {
            key: root.fnActive ? Qt.Key_F11 : Qt.Key_Minus
            text: root.fnActive ? "" : (root.latinMode ? "-" : "ㄦ")
            displayText: root.fnActive ? "F11" : (root.latinMode ? "-" : "ㄦ")
            alternativeKeys: root.fnActive ? [] : (root.latinMode ? "-_" : "ㄦ-_")
            smallText: root.fnActive ? "" : (root.latinMode ? "_" : "-")
        }
        Key {
            key: root.fnActive ? Qt.Key_F12 : Qt.Key_Equal
            text: root.fnActive ? "" : "="
            displayText: root.fnActive ? "F12" : "="
            alternativeKeys: root.fnActive ? [] : "=+"
            smallText: root.fnActive ? "" : "+"
        }
        BackspaceKey { weight: root.defaultKeyWeight * 1.5 }
    }

    KeyboardRow {
        KeysymKey { key: Qt.Key_Tab; displayText: "Tab"; weight: root.defaultKeyWeight * 1.5 }
        Key { key: Qt.Key_Q; text: (root.latinMode ? "q" : "ㄆ") }
        Key { key: Qt.Key_W; text: (root.latinMode ? "w" : "ㄊ") }
        Key { key: Qt.Key_E; text: (root.latinMode ? "e" : "ㄍ") }
        Key { key: Qt.Key_R; text: (root.latinMode ? "r" : "ㄐ") }
        Key { key: Qt.Key_T; text: (root.latinMode ? "t" : "ㄔ") }
        Key { key: Qt.Key_Y; text: (root.latinMode ? "y" : "ㄗ") }
        Key { key: Qt.Key_U; text: (root.latinMode ? "u" : "ㄧ") }
        Key { key: Qt.Key_I; text: (root.latinMode ? "i" : "ㄛ") }
        Key { key: Qt.Key_O; text: (root.latinMode ? "o" : "ㄟ") }
        Key { key: Qt.Key_P; text: (root.latinMode ? "p" : "ㄣ") }
        Key { key: Qt.Key_BracketLeft; text: "["; alternativeKeys: "[{"; smallText: "{" }
        Key { key: Qt.Key_BracketRight; text: "]"; alternativeKeys: "]}"; smallText: "}" }
        Key { key: Qt.Key_Backslash; text: "\\"; alternativeKeys: "\\|"; smallText: "|" }
        KeysymKey { key: Qt.Key_Delete; displayText: "Del" }
    }

    KeyboardRow {
        KeysymKey {
            key: Qt.Key_CapsLock
            displayText: "Caps"
            weight: root.defaultKeyWeight * 1.75
            highlighted: inputEngine && inputEngine.capsLockActive
            onClicked: {
                if (!inputEngine) {
                    return;
                }
                inputEngine.capsLockActive = !inputEngine.capsLockActive;
                inputEngine.shiftActive = false;
            }
        }
        Key { key: Qt.Key_A; text: (root.latinMode ? "a" : "ㄇ") }
        Key { key: Qt.Key_S; text: (root.latinMode ? "s" : "ㄋ") }
        Key { key: Qt.Key_D; text: (root.latinMode ? "d" : "ㄎ") }
        Key { key: Qt.Key_F; text: (root.latinMode ? "f" : "ㄑ") }
        Key { key: Qt.Key_G; text: (root.latinMode ? "g" : "ㄕ") }
        Key { key: Qt.Key_H; text: (root.latinMode ? "h" : "ㄘ") }
        Key { key: Qt.Key_J; text: (root.latinMode ? "j" : "ㄨ") }
        Key { key: Qt.Key_K; text: (root.latinMode ? "k" : "ㄜ") }
        Key { key: Qt.Key_L; text: (root.latinMode ? "l" : "ㄠ") }
        Key { key: Qt.Key_Semicolon; text: (root.latinMode ? ";" : "ㄤ"); alternativeKeys: (root.latinMode ? ";:" : "ㄤ;:"); smallText: ":" }
        Key { key: Qt.Key_Apostrophe; text: "'"; alternativeKeys: "'\""; smallText: "\"" }
        EnterKey { weight: root.defaultKeyWeight * 2.75 }
    }

    KeyboardRow {
        ShiftKey { weight: root.defaultKeyWeight * 2.5; sendDirectKeyEvents: true }
        Key { key: Qt.Key_Z; text: (root.latinMode ? "z" : "ㄈ") }
        Key { key: Qt.Key_X; text: (root.latinMode ? "x" : "ㄌ") }
        Key { key: Qt.Key_C; text: (root.latinMode ? "c" : "ㄏ") }
        Key { key: Qt.Key_V; text: (root.latinMode ? "v" : "ㄒ") }
        Key { key: Qt.Key_B; text: (root.latinMode ? "b" : "ㄖ") }
        Key { key: Qt.Key_N; text: (root.latinMode ? "n" : "ㄙ") }
        Key { key: Qt.Key_M; text: (root.latinMode ? "m" : "ㄩ") }
        Key { key: Qt.Key_Comma; text: (root.latinMode ? "," : "ㄝ"); alternativeKeys: (root.latinMode ? ",<" : "ㄝ,<"); smallText: "<" }
        Key { key: Qt.Key_Period; text: (root.latinMode ? "." : "ㄡ"); alternativeKeys: (root.latinMode ? ".>" : "ㄡ.>"); smallText: ">" }
        Key { key: Qt.Key_Slash; text: (root.latinMode ? "/" : "ㄥ"); alternativeKeys: (root.latinMode ? "/?" : "ㄥ/?"); smallText: "?" }
        KeysymKey { key: Qt.Key_Up; iconName: "go-up-symbolic"; displayText: ""; repeat: true }
        ShiftKey { weight: root.defaultKeyWeight * 2; sendDirectKeyEvents: true }
    }

    KeyboardRow {
        KeysymKey { key: Qt.Key_Control; displayText: "Ctrl"; sticky: true }
        KeysymKey { key: KeyCodes.KeyFn; displayText: "Fn"; sticky: true }
        KeysymKey { key: Qt.Key_Meta; iconName: "start-here-symbolic"; displayText: ""; sticky: true }
        KeysymKey { key: Qt.Key_Alt; displayText: "Alt"; sticky: true }
        InputModeKey {
            textComposer: root.textComposer
            modes: [
                { value: "zhuyin", label: "中" },
                { value: "latin", label: "英" }
            ]
        }
        SpaceKey { weight: root.defaultKeyWeight * 6.5 }
        KeysymKey { key: Qt.Key_Left; iconName: "go-previous-symbolic"; displayText: ""; repeat: true }
        KeysymKey { key: Qt.Key_Down; iconName: "go-down-symbolic"; displayText: ""; repeat: true }
        KeysymKey { key: Qt.Key_Right; iconName: "go-next-symbolic"; displayText: ""; repeat: true }
        ChangeLanguageKey { weight: root.defaultKeyWeight / 2 }
        HideKeyboardKey { weight: root.defaultKeyWeight / 2 }
    }
}
