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
    property var inputModeKeyModes: []
    readonly property bool inputModeKeyVisible: inputModeKeyModes.length > 0
    property var keyTextMap: ({})
    property var shiftedKeyTextMap: ({})
    property bool sendShiftDirectKeyEvents: true

    function keyText(text) {
        const shifted = inputEngine && inputEngine.uppercase;
        const map = shifted ? shiftedKeyTextMap : keyTextMap;
        return map[text] ?? keyTextMap[text] ?? text;
    }

    // 5 rows instead of 4, and the top row is 75% of a typical row height
    panelHeightFactor: 1.1875

    // Keep normal key slots square: 15.5 key-width units across / 5 key-height units tall.
    maxWidthToHeightRatio: (fullRowWeight / defaultKeyWeight) / rowCount

    KeyboardRow {
        heightWeight: defaultRowHeightWeight * 0.75

        KeysymKey { key: Qt.Key_Escape; displayText: "Esc" }
        Key { key: Qt.Key_QuoteLeft; text: "`"; alternativeKeys: "`~"; smallText: "~" }
        Key {
            key: root.fnActive ? Qt.Key_F1 : Qt.Key_1
            text: root.fnActive ? "" : root.keyText("1")
            displayText: root.fnActive ? "F1" : root.keyText("1")
            alternativeKeys: root.fnActive ? [] : "1!"
            smallText: root.fnActive ? "" : "!"
        }
        Key {
            key: root.fnActive ? Qt.Key_F2 : Qt.Key_2
            text: root.fnActive ? "" : root.keyText("2")
            displayText: root.fnActive ? "F2" : root.keyText("2")
            alternativeKeys: root.fnActive ? [] : "2@"
            smallText: root.fnActive ? "" : "@"
        }
        Key {
            key: root.fnActive ? Qt.Key_F3 : Qt.Key_3
            text: root.fnActive ? "" : root.keyText("3")
            displayText: root.fnActive ? "F3" : root.keyText("3")
            alternativeKeys: root.fnActive ? [] : "3#"
            smallText: root.fnActive ? "" : "#"
        }
        Key {
            key: root.fnActive ? Qt.Key_F4 : Qt.Key_4
            text: root.fnActive ? "" : root.keyText("4")
            displayText: root.fnActive ? "F4" : root.keyText("4")
            alternativeKeys: root.fnActive ? [] : "4$"
            smallText: root.fnActive ? "" : "$"
        }
        Key {
            key: root.fnActive ? Qt.Key_F5 : Qt.Key_5
            text: root.fnActive ? "" : root.keyText("5")
            displayText: root.fnActive ? "F5" : root.keyText("5")
            alternativeKeys: root.fnActive ? [] : "5%"
            smallText: root.fnActive ? "" : "%"
        }
        Key {
            key: root.fnActive ? Qt.Key_F6 : Qt.Key_6
            text: root.fnActive ? "" : root.keyText("6")
            displayText: root.fnActive ? "F6" : root.keyText("6")
            alternativeKeys: root.fnActive ? [] : "6^"
            smallText: root.fnActive ? "" : "^"
        }
        Key {
            key: root.fnActive ? Qt.Key_F7 : Qt.Key_7
            text: root.fnActive ? "" : root.keyText("7")
            displayText: root.fnActive ? "F7" : root.keyText("7")
            alternativeKeys: root.fnActive ? [] : "7&"
            smallText: root.fnActive ? "" : "&"
        }
        Key {
            key: root.fnActive ? Qt.Key_F8 : Qt.Key_8
            text: root.fnActive ? "" : root.keyText("8")
            displayText: root.fnActive ? "F8" : root.keyText("8")
            alternativeKeys: root.fnActive ? [] : "8*"
            smallText: root.fnActive ? "" : "*"
        }
        Key {
            key: root.fnActive ? Qt.Key_F9 : Qt.Key_9
            text: root.fnActive ? "" : root.keyText("9")
            displayText: root.fnActive ? "F9" : root.keyText("9")
            alternativeKeys: root.fnActive ? [] : "9("
            smallText: root.fnActive ? "" : "("
        }
        Key {
            key: root.fnActive ? Qt.Key_F10 : Qt.Key_0
            text: root.fnActive ? "" : root.keyText("0")
            displayText: root.fnActive ? "F10" : root.keyText("0")
            alternativeKeys: root.fnActive ? [] : "0)"
            smallText: root.fnActive ? "" : ")"
        }
        Key {
            key: root.fnActive ? Qt.Key_F11 : Qt.Key_Minus
            text: root.fnActive ? "" : root.keyText("-")
            displayText: root.fnActive ? "F11" : root.keyText("-")
            alternativeKeys: root.fnActive ? [] : "-_"
            smallText: root.fnActive ? "" : "_"
        }
        Key {
            key: root.fnActive ? Qt.Key_F12 : Qt.Key_Equal
            text: root.fnActive ? "" : root.keyText("=")
            displayText: root.fnActive ? "F12" : root.keyText("=")
            alternativeKeys: root.fnActive ? [] : "=+"
            smallText: root.fnActive ? "" : "+"
        }
        BackspaceKey { weight: root.defaultKeyWeight * 1.5 }
    }

    KeyboardRow {
        KeysymKey { key: Qt.Key_Tab; displayText: "Tab"; weight: root.defaultKeyWeight * 1.5 }
        Key { key: Qt.Key_Q; text: root.keyText("q") }
        Key { key: Qt.Key_W; text: root.keyText("w") }
        Key { key: Qt.Key_E; text: root.keyText("e") }
        Key { key: Qt.Key_R; text: root.keyText("r") }
        Key { key: Qt.Key_T; text: root.keyText("t") }
        Key { key: Qt.Key_Y; text: root.keyText("y") }
        Key { key: Qt.Key_U; text: root.keyText("u") }
        Key { key: Qt.Key_I; text: root.keyText("i") }
        Key { key: Qt.Key_O; text: root.keyText("o") }
        Key { key: Qt.Key_P; text: root.keyText("p") }
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
        Key { key: Qt.Key_A; text: root.keyText("a") }
        Key { key: Qt.Key_S; text: root.keyText("s") }
        Key { key: Qt.Key_D; text: root.keyText("d") }
        Key { key: Qt.Key_F; text: root.keyText("f") }
        Key { key: Qt.Key_G; text: root.keyText("g") }
        Key { key: Qt.Key_H; text: root.keyText("h") }
        Key { key: Qt.Key_J; text: root.keyText("j") }
        Key { key: Qt.Key_K; text: root.keyText("k") }
        Key { key: Qt.Key_L; text: root.keyText("l") }
        Key { key: Qt.Key_Semicolon; text: root.keyText(";"); alternativeKeys: ";:"; smallText: ":" }
        Key { key: Qt.Key_Apostrophe; text: root.keyText("'"); alternativeKeys: "'\""; smallText: "\"" }
        EnterKey { weight: root.defaultKeyWeight * 2.75 }
    }

    KeyboardRow {
        ShiftKey { weight: root.defaultKeyWeight * 2.5; sendDirectKeyEvents: root.sendShiftDirectKeyEvents }
        Key { key: Qt.Key_Z; text: root.keyText("z") }
        Key { key: Qt.Key_X; text: root.keyText("x") }
        Key { key: Qt.Key_C; text: root.keyText("c") }
        Key { key: Qt.Key_V; text: root.keyText("v") }
        Key { key: Qt.Key_B; text: root.keyText("b") }
        Key { key: Qt.Key_N; text: root.keyText("n") }
        Key { key: Qt.Key_M; text: root.keyText("m") }
        Key { key: Qt.Key_Comma; text: root.keyText(","); alternativeKeys: ",<"; smallText: "<" }
        Key { key: Qt.Key_Period; text: root.keyText("."); alternativeKeys: ".>"; smallText: ">" }
        Key { key: Qt.Key_Slash; text: root.keyText("/"); alternativeKeys: "/?"; smallText: "?" }
        KeysymKey { key: Qt.Key_Up; iconName: "go-up-symbolic"; displayText: ""; repeat: true }
        ShiftKey { weight: root.defaultKeyWeight * 2; sendDirectKeyEvents: root.sendShiftDirectKeyEvents }
    }

    KeyboardRow {
        KeysymKey { key: Qt.Key_Control; displayText: "Ctrl"; sticky: true }
        KeysymKey { key: KeyCodes.KeyFn; displayText: "Fn"; sticky: true }
        KeysymKey { key: Qt.Key_Meta; iconName: "start-here-symbolic"; displayText: ""; sticky: true }
        KeysymKey { key: Qt.Key_Alt; displayText: "Alt"; sticky: true }
        InputModeKey {
            visible: root.inputModeKeyVisible
            height: visible ? implicitHeight : 0
            weight: visible ? root.defaultKeyWeight : 0
            textComposer: root.textComposer
            modes: root.inputModeKeyModes
        }
        SpaceKey { weight: root.defaultKeyWeight * (root.inputModeKeyVisible ? 6.5 : 7.5) }
        KeysymKey { key: Qt.Key_Left; iconName: "go-previous-symbolic"; displayText: ""; repeat: true }
        KeysymKey { key: Qt.Key_Down; iconName: "go-down-symbolic"; displayText: ""; repeat: true }
        KeysymKey { key: Qt.Key_Right; iconName: "go-next-symbolic"; displayText: ""; repeat: true }
        ChangeLanguageKey { weight: root.defaultKeyWeight / 2 }
        HideKeyboardKey { weight: root.defaultKeyWeight / 2 }
    }
}
