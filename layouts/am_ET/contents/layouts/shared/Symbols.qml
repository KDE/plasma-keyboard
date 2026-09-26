/*
    SPDX-FileCopyrightText: 2026 Abenezer Wesenseged <wseged@proton.me>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

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
                Key { text: "፩"; alternativeKeys: "1\u1369\u1373"; smallText: "፳" }
                Key { text: "፪"; alternativeKeys: "2\u136A\u1374"; smallText: "፴" }
                Key { text: "፫"; alternativeKeys: "3\u136B\u1375"; smallText: "፵" }
                Key { text: "፬"; alternativeKeys: "4\u136C\u1376"; smallText: "፶" }
                Key { text: "፭"; alternativeKeys: "5\u136D\u1377"; smallText: "፷" }
                Key { text: "፮"; alternativeKeys: "6\u136E\u1378"; smallText: "፸" }
                Key { text: "፯"; alternativeKeys: "7\u136F\u1379"; smallText: "፹" }
                Key { text: "፰"; alternativeKeys: "8\u1370\u137A"; smallText: "፺" }
                Key { text: "፱"; alternativeKeys: "9\u1371\u137B"; smallText: "፻" }
                Key { text: "፲"; alternativeKeys: "0\u1372\u137C"; smallText: "፼" }
            }

            KeyboardRow {
                Key { text: "፡"; alternativeKeys: "@"; smallText: "@" }
                Key { text: "፧"; alternativeKeys: "#"; smallText: "#" }
                Key { text: "፨"; alternativeKeys: "%"; smallText: "%" }
                Key { text: "፠"; alternativeKeys: "&"; smallText: "&" }
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
                Key { text: "፥"; alternativeKeys: "<"; smallText: "<" }
                Key { text: "፦"; alternativeKeys: ">"; smallText: ">" }
                Key { key: Qt.Key_Apostrophe; text: "'" }
                Key { key: Qt.Key_Colon; text: ":" }
                Key { key: Qt.Key_Slash; text: "/" }
                Key { key: Qt.Key_Exclam; text: "!" }
                Key { key: Qt.Key_Question; text: "?" }
                BackspaceKey {}
            }

            KeyboardRow {
                SymbolModeKey { weight: 150; displayText: "ሀለሐ" }
                ChangeLanguageKey {}
                Key { text: ","; secondaryStyle: true }
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
                Key { key: 0xA5; text: "¥" }
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
                Key { key: Qt.Key_Semicolon; text: "፤"; alternativeKeys: ";"; smallText: ";" }
                Key { key: 0x201C; text: '“' }
                Key { key: 0x201D; text: '”' }
                Key { key: Qt.Key_Backslash; text: "\\" }
                BackspaceKey {}
            }

            KeyboardRow {
                SymbolModeKey { weight: 150; displayText: "ሀለሐ" }
                ChangeLanguageKey {}
                Key { text: ","; secondaryStyle: true }
                SpaceKey { weight: 500 }
                Key { text: "…"; alternativeKeys: "…!.?"; secondaryStyle: true }
                HideKeyboardKey {}
                EnterKey { weight: 150 }
            }
        }
    }
}
