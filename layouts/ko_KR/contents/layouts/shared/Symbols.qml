// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayoutLoader {
    property bool secondPage
    onVisibleChanged: if (!visible) secondPage = false
    sourceComponent: secondPage ? page2 : page1

    Component {
        id: page1

        KeyboardLayout {
            sharedLayouts: ["main"]
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
                Key { key: Qt.Key_NumberSign; text: "#" }
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
                Key { displayText: "1/2"; functionKey: true; noKeyEvent: true; onClicked: secondPage = true; secondaryStyle: true }
                Key { key: Qt.Key_QuoteDbl; text: "\"" }
                Key { key: Qt.Key_Less; text: "<" }
                Key { key: Qt.Key_Greater; text: ">" }
                Key { key: Qt.Key_Apostrophe; text: "'" }
                Key { key: Qt.Key_Colon; text: ":" }
                Key { key: Qt.Key_Slash; text: "/" }
                Key { key: Qt.Key_Exclam; text: "!" }
                Key { key: Qt.Key_Question; text: "?" }
                BackspaceKey {}
            }

            KeyboardRow {
                SymbolModeKey { weight: functionKeyWeight; displayText: "\uD55C\uAE00" }
                ChangeLanguageKey { weight: defaultKeyWeight }
                Key { key: Qt.Key_Comma; weight: defaultKeyWeight; text: ","; secondaryStyle: true }
                SpaceKey { weight: defaultKeyWeight * 3 }
                Key { key: Qt.Key_Period; weight: defaultKeyWeight; text: "."; secondaryStyle: true }
                HideKeyboardKey { weight: defaultKeyWeight }
                EnterKey { weight: functionKeyWeight }
            }
        }
    }

    Component {
        id: page2

        KeyboardLayout {
            sharedLayouts: ["main"]
            readonly property real functionKeyWeight: defaultKeyWeight * 1.5

            KeyboardRow {
                Key { key: Qt.Key_AsciiTilde; text: "~" }
                Key { key: Qt.Key_Agrave; text: "`" }
                Key { key: Qt.Key_Bar; text: "|" }
                Key { key: 0x00B7; text: "·" }
                Key { key: 0x221A; text: "√" }
                Key { key: Qt.Key_division; text: "÷" }
                Key { key: Qt.Key_multiply; text: "×" }
                Key { key: Qt.Key_onehalf; text: "½"; alternativeKeys: "¼⅓½¾⅞" }
                Key { key: Qt.Key_BraceLeft; text: "{" }
                Key { key: Qt.Key_BraceRight; text: "}" }
            }

            KeyboardRow {
                Key { key: Qt.Key_Dollar; text: "$" }
                Key { key: 0x20AC; text: "€" }
                Key { key: 0x00A3; text: "£" }
                Key { key: 0x20A9; text: "\u20A9" }
                Key { key: 0x00A5; text: "¥" }
                Key { key: Qt.Key_AsciiCircum; text: "^" }
                Key { key: Qt.Key_Equal; text: "=" }
                Key { key: Qt.Key_section; text: "§" }
                Key { key: Qt.Key_BracketLeft; text: "[" }
                Key { key: Qt.Key_BracketRight; text: "]" }
            }

            KeyboardRow {
                Key { displayText: "2/2"; functionKey: true; noKeyEvent: true; onClicked: secondPage = false; secondaryStyle: true }
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
                SymbolModeKey { weight: functionKeyWeight; displayText: "\uD55C\uAE00" }
                ChangeLanguageKey { weight: defaultKeyWeight }
                Key { key: Qt.Key_Comma; weight: defaultKeyWeight; text: ","; secondaryStyle: true }
                SpaceKey { weight: defaultKeyWeight * 3 }
                Key { key: 0x2026; weight: defaultKeyWeight; text: "\u2026"; secondaryStyle: true }
                HideKeyboardKey { weight: defaultKeyWeight }
                EnterKey { weight: functionKeyWeight }
            }
        }
    }
}
