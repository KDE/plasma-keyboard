// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    readonly property int rowCount: 4
    readonly property int fullRowWeight: 1300
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

    function keyAlternativeKeys(text, alternativeKeys, numberIndex) {
        const mappedText = keyText(text);
        if (mappedText === text) {
            return alternativeKeys;
        }
        return alternativeKeys.slice(0, numberIndex) + mappedText + alternativeKeys.slice(numberIndex);
    }

    // Keep normal key slots square: 13 key width units across / 4 key height units tall
    maxWidthToHeightRatio: (fullRowWeight / defaultKeyWeight) / rowCount

    KeyboardRow {
        KeysymKey { key: Qt.Key_Escape; displayText: "Esc"; }
        Key { key: Qt.Key_Q; text: root.keyText("q"); alternativeKeys: root.keyAlternativeKeys("q", "q1", 1); smallText: "1"}
        Key { key: Qt.Key_W; text: root.keyText("w"); alternativeKeys: root.keyAlternativeKeys("w", "w2", 1); smallText: "2"}
        Key { key: Qt.Key_E; text: root.keyText("e"); alternativeKeys: root.keyAlternativeKeys("e", "êe3ëèé", 2); smallText: "3"}
        Key { key: Qt.Key_R; text: root.keyText("r"); alternativeKeys: root.keyAlternativeKeys("r", "ŕr4ř", 2); smallText: "4"}
        Key { key: Qt.Key_T; text: root.keyText("t"); alternativeKeys: root.keyAlternativeKeys("t", "ţt5ŧť", 2); smallText: "5"}
        Key { key: Qt.Key_Y; text: root.keyText("y"); alternativeKeys: root.keyAlternativeKeys("y", "ÿy6ýŷ", 2); smallText: "6"}
        Key { key: Qt.Key_U; text: root.keyText("u"); alternativeKeys: root.keyAlternativeKeys("u", "űūũûüu7ùú", 6); smallText: "7"}
        Key { key: Qt.Key_I; text: root.keyText("i"); alternativeKeys: root.keyAlternativeKeys("i", "îïīĩi8ìí", 5); smallText: "8"}
        Key { key: Qt.Key_O; text: root.keyText("o"); alternativeKeys: root.keyAlternativeKeys("o", "œøõôöòóo9", 8); smallText: "9"}
        Key { key: Qt.Key_P; text: root.keyText("p"); alternativeKeys: root.keyAlternativeKeys("p", "p0", 1); smallText: "0"}
        BackspaceKey { weight: root.defaultKeyWeight * 2 }
    }

    KeyboardRow {
        KeysymKey { weight: defaultKeyWeight * 1.5; key: Qt.Key_Tab; displayText: "Tab" }
        Key { key: Qt.Key_A; text: root.keyText("a"); alternativeKeys: "aäåãâàá" }
        Key { key: Qt.Key_S; text: root.keyText("s"); alternativeKeys: "šsşś" }
        Key { key: Qt.Key_D; text: root.keyText("d"); alternativeKeys: "dđď" }
        Key { key: Qt.Key_F; text: root.keyText("f") }
        Key { key: Qt.Key_G; text: root.keyText("g"); alternativeKeys: "ġgģĝğ" }
        Key { key: Qt.Key_H; text: root.keyText("h") }
        Key { key: Qt.Key_J; text: root.keyText("j") }
        Key { key: Qt.Key_K; text: root.keyText("k") }
        Key { key: Qt.Key_L; text: root.keyText("l"); alternativeKeys: "ĺŀłļľl" }
        Key { key: Qt.Key_Apostrophe; text: root.keyText("'"); alternativeKeys: "\""; smallText: "\""}
        EnterKey { weight: root.defaultKeyWeight * 1.5 }
    }

    KeyboardRow {
        ShiftKey { weight: root.defaultKeyWeight * 2; sendDirectKeyEvents: root.sendShiftDirectKeyEvents }
        Key { key: Qt.Key_Z; text: root.keyText("z"); alternativeKeys: "zžż" }
        Key { key: Qt.Key_X; text: root.keyText("x") }
        Key { key: Qt.Key_C; text: root.keyText("c"); alternativeKeys: "çcċčć" }
        Key { key: Qt.Key_V; text: root.keyText("v") }
        Key { key: Qt.Key_B; text: root.keyText("b") }
        Key { key: Qt.Key_N; text: root.keyText("n"); alternativeKeys: "ņńnň" }
        Key { key: Qt.Key_M; text: root.keyText("m") }
        Key { key: Qt.Key_Comma; text: root.keyText(","); alternativeKeys: ";"; smallText: ";"}
        Key { key: Qt.Key_Period; text: root.keyText("."); alternativeKeys: ":"; smallText: ":"}
        Key { key: Qt.Key_Question; text: root.keyText("?"); alternativeKeys: "!"; smallText: "!"}
        ShiftKey { sendDirectKeyEvents: root.sendShiftDirectKeyEvents }
    }

    KeyboardRow {
        SymbolModeKey { }
        KeysymKey { key: Qt.Key_Control; displayText: "Ctrl"; sticky: true }
        KeysymKey { key: Qt.Key_Meta; iconName: "start-here-symbolic"; displayText: ""; sticky: true }
        KeysymKey { key: Qt.Key_Alt; displayText: "Alt"; sticky: true }
        InputModeKey {
            visible: root.inputModeKeyVisible
            height: visible ? implicitHeight : 0
            weight: visible ? root.defaultKeyWeight : 0
            textComposer: root.textComposer
            modes: root.inputModeKeyModes
        }
        SpaceKey { weight: root.defaultKeyWeight * (root.inputModeKeyVisible ? 4 : 5) }
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
