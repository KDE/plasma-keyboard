// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayoutLoader {
    id: root

    property bool secondPage
    onVisibleChanged: if (!visible) secondPage = false
    sourceComponent: root.secondPage ? page2 : page1

    Component {
        id: page1
        KeyboardLayout {
            id: page1Layout
            textComposer: DirectTextComposer {}
            readonly property real functionKeyWeight: defaultKeyWeight * 1.5
            KeyboardRow {
                Key { key: Qt.Key_1; text: "1" }
                Key { key: Qt.Key_2; text: "2" }
                Key { key: Qt.Key_3; text: "3" }
                Key { key: Qt.Key_4; text: "4" }
                Key { key: Qt.Key_5; text: "5" }
                Key { key: Qt.Key_6; text: "6" }
                Key { key: Qt.Key_7; text: "7" }
                Key { key: Qt.Key_8; text: "8" }
                Key { key: Qt.Key_9; text: "9" }
                Key { key: Qt.Key_0; text: "0" }
            }
            KeyboardRow {
                Key { key: Qt.Key_At; text: "@" }
                Key { key: 0x2116; text: "№"; alternativeKeys: "№#" }
                Key { key: Qt.Key_Percent; text: "%" }
                Key { key: Qt.Key_Ampersand; text: "&" }
                Key { key: Qt.Key_Asterisk; text: "*" }
                Key { key: Qt.Key_Underscore; text: "_" }
                Key { key: Qt.Key_Minus; text: "-" }
                Key { key: Qt.Key_Plus; text: "+" }
                Key { key: Qt.Key_ParenLeft; text: "(" }
                Key { key: Qt.Key_ParenRight; text: ")" }
            }
            KeyboardRow {
                Key { displayText: "1/2"; functionKey: true; noKeyEvent: true; onClicked: root.secondPage = !root.secondPage; secondaryStyle: true }
                Key { key: Qt.Key_QuoteDbl; text: "\"" }
                Key { key: Qt.Key_Less; text: "<" }
                Key { key: Qt.Key_Greater; text: ">" }
                Key { key: Qt.Key_Apostrophe; text: "'" }
                Key { key: Qt.Key_Colon; text: ":" }
                Key { key: Qt.Key_Slash; text: "/" }
                Key {
                    key: Qt.Key_Exclam
                    text: "!"
                    alternativeKeys: []
                }
                Key {
                    key: Qt.Key_Question
                    text: "?"
                    alternativeKeys: []
                }
                BackspaceKey {}
            }
            KeyboardRow {
                SymbolModeKey { weight: page1Layout.functionKeyWeight; displayText: "АБВ" }
                ChangeLanguageKey { weight: page1Layout.defaultKeyWeight }
                Key { key: Qt.Key_Comma; weight: page1Layout.defaultKeyWeight; text: ","; secondaryStyle: true }
                SpaceKey { weight: page1Layout.defaultKeyWeight * 3 }
                Key { key: Qt.Key_Period; weight: page1Layout.defaultKeyWeight; text: "."; secondaryStyle: true }
                HideKeyboardKey { weight: page1Layout.defaultKeyWeight }
                EnterKey { weight: page1Layout.functionKeyWeight }
            }
        }
    }

    Component {
        id: page2
        KeyboardLayout {
            id: page2Layout
            textComposer: DirectTextComposer {}
            readonly property real functionKeyWeight: defaultKeyWeight * 1.5
            KeyboardRow {
                Key { key: Qt.Key_AsciiTilde; text: "~" }
                Key { key: Qt.Key_QuoteLeft; text: "`" }
                Key { key: Qt.Key_Bar; text: "|" }
                Key { key: 0xB7; text: "·" }
                Key { key: 0x221A; text: "√" }
                Key { key: Qt.Key_division; text: "÷" }
                Key { key: Qt.Key_multiply; text: "×" }
                Key { key: Qt.Key_onehalf; text: "½"; alternativeKeys: "¼⅓½¾⅞" }
                Key { key: Qt.Key_BraceLeft; text: "{" }
                Key { key: Qt.Key_BraceRight; text: "}" }
            }
            KeyboardRow {
                Key { key: 0x20BD; text: "₽"; alternativeKeys: "₽$" }
                Key { key: 0x20AC; text: "€" }
                Key { key: 0xA3; text: "£" }
                Key { key: 0xA2; text: "¢" }
                Key { key: 0xA5; text: "¥" }
                Key { key: Qt.Key_AsciiCircum; text: "^" }
                Key { key: Qt.Key_Equal; text: "=" }
                Key { key: Qt.Key_section; text: "§" }
                Key { key: Qt.Key_BracketLeft; text: "[" }
                Key { key: Qt.Key_BracketRight; text: "]" }
            }
            KeyboardRow {
                Key { displayText: "2/2"; functionKey: true; noKeyEvent: true; onClicked: root.secondPage = !root.secondPage; secondaryStyle: true }
                Key { key: 0x2122; text: "™" }
                Key { key: 0x00AE; text: "®" }
                Key { key: Qt.Key_guillemotleft; text: "«" }
                Key { key: Qt.Key_guillemotright; text: "»" }
                Key { key: Qt.Key_Semicolon; text: ";" }
                Key { key: 0x201C; text: "“" }
                Key { key: 0x201D; text: "”" }
                Key { key: Qt.Key_Backslash; text: "\\" }
                BackspaceKey {}
            }
            KeyboardRow {
                SymbolModeKey { weight: page2Layout.functionKeyWeight; displayText: "АБВ" }
                ChangeLanguageKey { weight: page2Layout.defaultKeyWeight }
                Key { key: Qt.Key_Comma; weight: page2Layout.defaultKeyWeight; text: ","; secondaryStyle: true }
                SpaceKey { weight: page2Layout.defaultKeyWeight * 3 }
                Key { key: 0x2026; weight: page2Layout.defaultKeyWeight; text: "\u2026"; secondaryStyle: true }
                HideKeyboardKey { weight: page2Layout.defaultKeyWeight }
                EnterKey { weight: page2Layout.functionKeyWeight }
            }
        }
    }
}
