// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayoutLoader {
    id: root

    property bool secondPage: false
    onVisibleChanged: if (!visible) secondPage = false
    sourceComponent: secondPage ? page2 : page1

    Component {
        id: page1
        KeyboardLayout {
            textComposer: DirectTextComposer {}

            KeyboardRow {
                Key { text: "۱"; alternativeKeys: "\u06F11" }
                Key { text: "۲"; alternativeKeys: "\u06F22" }
                Key { text: "۳"; alternativeKeys: "\u06F33" }
                Key { text: "۴"; alternativeKeys: "\u06F44" }
                Key { text: "۵"; alternativeKeys: "\u06F55" }
                Key { text: "۶"; alternativeKeys: "\u06F66" }
                Key { text: "۷"; alternativeKeys: "\u06F77" }
                Key { text: "۸"; alternativeKeys: "\u06F88" }
                Key { text: "۹"; alternativeKeys: "\u06F99" }
                Key { text: "۰"; alternativeKeys: "\u06F00" }
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
                Key { displayText: "1/2"; functionKey: true; onClicked: root.secondPage = !root.secondPage }
                Key { key: Qt.Key_QuoteDbl; text: '"' }
                Key { key: Qt.Key_Less; text: "<" }
                Key { key: Qt.Key_Greater; text: ">" }
                Key { key: Qt.Key_Apostrophe; text: "'" }
                Key { key: Qt.Key_Semicolon; text: "؛"; alternativeKeys: "\u061B;" }
                Key { key: Qt.Key_Slash; text: "/" }
                Key { key: Qt.Key_Exclam; text: "!" }
                Key { key: Qt.Key_Question; text: "؟"; alternativeKeys: "\u061F?" }
                BackspaceKey {}
            }

            KeyboardRow {
                SymbolModeKey { weight: 150; displayText: "ابپ" }
                ChangeLanguageKey {}
                Key { text: "،"; secondaryStyle: true }
                SpaceKey { weight: 500 }
                Key { text: "."; alternativeKeys: ".!.?"; secondaryStyle: true }
                HideKeyboardKey {}
                EnterKey { weight: 150 }
            }
        }
    }

    Component {
        id: page2
        KeyboardLayout {
            textComposer: DirectTextComposer {}

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
                Key { key: Qt.Key_Dollar; text: "$" }
                Key { key: 0x20AC; text: "€" }
                Key { key: 0xA3; text: "£" }
                Key { key: 0xA2; text: "¢" }
                Key { key: 0xFDFC; text: "﷼" }
                Key { key: Qt.Key_AsciiCircum; text: "^" }
                Key { key: Qt.Key_Equal; text: "=" }
                Key { key: Qt.Key_section; text: "§" }
                Key { key: Qt.Key_BracketLeft; text: "[" }
                Key { key: Qt.Key_BracketRight; text: "]" }
            }

            KeyboardRow {
                Key { displayText: "2/2"; functionKey: true; onClicked: root.secondPage = !root.secondPage }
                Key { key: 0x2122; text: '™' }
                Key { key: 0x00AE; text: '®' }
                Key { key: Qt.Key_guillemotleft; text: '«' }
                Key { key: Qt.Key_guillemotright; text: '»' }
                Key { key: Qt.Key_Colon; text: ":" }
                Key { key: 0x201C; text: '“' }
                Key { key: 0x201D; text: '”' }
                Key { key: Qt.Key_Backslash; text: "\\" }
                BackspaceKey {}
            }

            KeyboardRow {
                SymbolModeKey { weight: 150; displayText: "ابپ" }
                ChangeLanguageKey {}
                Key { text: "،"; secondaryStyle: true }
                SpaceKey { weight: 500 }
                Key { text: "…"; alternativeKeys: "…!.?"; secondaryStyle: true }
                HideKeyboardKey {}
                EnterKey { weight: 150 }
            }
        }
    }
}
