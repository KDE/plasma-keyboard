/*
    SPDX-FileCopyrightText: 2026 Abenezer Wesenseged <wseged@proton.me>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Components

KeyboardLayoutLoader {
    property bool secondPage
    onVisibleChanged: if (!visible)
        secondPage = false
    sourceComponent: secondPage ? page2 : page1
    Component {
        id: page1
        KeyboardLayout {
            keyWeight: 160
            readonly property real normalKeyWidth: normalKey.width
            readonly property real functionKeyWidth: mapFromItem(normalKey, normalKey.width / 2, 0).x
            KeyboardRow {
                Key {
                    text: "\u1369"
                    alternativeKeys: "1\u1369\u1373"
                    smallText: "\u1373"
                    smallTextVisible: true
                }
                Key {
                    id: normalKey
                    text: "\u136A"
                    alternativeKeys: "2\u136A\u1374"
                    smallText: "\u1374"
                    smallTextVisible: true
                }
                Key {
                    text: "\u136B"
                    alternativeKeys: "3\u136B\u1375"
                    smallText: "\u1375"
                    smallTextVisible: true
                }
                Key {
                    text: "\u136C"
                    alternativeKeys: "4\u136C\u1376"
                    smallText: "\u1376"
                    smallTextVisible: true
                }
                Key {
                    text: "\u136D"
                    alternativeKeys: "5\u136D\u1377"
                    smallText: "\u1377"
                    smallTextVisible: true
                }
                Key {
                    text: "\u136E"
                    alternativeKeys: "6\u136E\u1378"
                    smallText: "\u1378"
                    smallTextVisible: true
                }
                Key {
                    text: "\u136F"
                    alternativeKeys: "7\u136F\u1379"
                    smallText: "\u1379"
                    smallTextVisible: true
                }
                Key {
                    text: "\u1370"
                    alternativeKeys: "8\u1370\u137A"
                    smallText: "\u137A"
                    smallTextVisible: true
                }
                Key {
                    text: "\u1371"
                    alternativeKeys: "9\u1371\u137B"
                    smallText: "\u137B"
                    smallTextVisible: true
                }
                Key {
                    text: "\u1372"
                    alternativeKeys: "0\u1372\u137C"
                    smallText: "\u137C"
                    smallTextVisible: true
                }
            }
            KeyboardRow {
                Key {
                    key: Qt.Key_At
                    text: "\u1361"
                    alternativeKeys: "@"
                    smallText: "@"
                }
                Key {
                    key: Qt.Key_NumberSign
                    text: "\u1367"
                    alternativeKeys: "#"
                    smallText: "#"
                }
                Key {
                    key: Qt.Key_Percent
                    text: "\u1368"
                    alternativeKeys: "%"
                    smallText: "%"
                    smallTextVisible: true
                }
                Key {
                    key: Qt.Key_Ampersand
                    text: "\u1360"
                    alternativeKeys: "&"
                    smallText: "&"
                    smallTextVisible: true
                }
                Key {
                    key: Qt.Key_Asterisk
                    text: "*"
                }
                Key {
                    key: Qt.Key_Underscore
                    text: "_"
                }
                Key {
                    key: Qt.Key_Minus
                    text: "-"
                }
                Key {
                    key: Qt.Key_Plus
                    text: "+"
                }
                Key {
                    key: Qt.Key_ParenLeft
                    text: "("
                }
                Key {
                    key: Qt.Key_ParenRight
                    text: ")"
                }
            }
            KeyboardRow {
                Key {
                    displayText: "1/2"
                    functionKey: true
                    onClicked: secondPage = !secondPage
                    highlighted: true
                }
                Key {
                    key: Qt.Key_QuoteDbl
                    text: '"'
                }
                Key {
                    key: Qt.Key_Less
                    text: "\u1365"
                    alternativeKeys: "<"
                    smallText: "<"
                    smallTextVisible: true
                }
                Key {
                    key: Qt.Key_Greater
                    text: "\u1366"
                    alternativeKeys: ">"
                    smallText: ">"
                    smallTextVisible: true
                }
                Key {
                    key: Qt.Key_Apostrophe
                    text: "'"
                }
                Key {
                    key: Qt.Key_Colon
                    text: ":"
                }
                Key {
                    key: Qt.Key_Slash
                    text: "/"
                }
                Key {
                    key: Qt.Key_Exclam
                    text: "!"
                }
                Key {
                    key: Qt.Key_Question
                    text: "?"
                }
                BackspaceKey {}
            }
            KeyboardRow {
                SymbolModeKey {
                    weight: functionKeyWidth
                    Layout.fillWidth: false
                    displayText: "\u1200\u1208\u1210"
                }
                ChangeLanguageKey {
                    weight: normalKeyWidth
                    Layout.fillWidth: false
                }
                Key {
                    weight: normalKeyWidth
                    Layout.fillWidth: false
                    text: ","
                    smallText: "\u2699"
                    smallTextVisible: keyboard.isFunctionPopupListAvailable()
                    highlighted: true
                }
                SpaceKey {}

                Key {
                    weight: normalKeyWidth
                    Layout.fillWidth: false
                    text: "."
                    highlighted: true
                }
                HideKeyboardKey {
                    weight: normalKeyWidth
                    Layout.fillWidth: false
                }
                EnterKey {
                    weight: functionKeyWidth
                    Layout.fillWidth: false
                }
            }
        }
    }
    Component {
        id: page2
        KeyboardLayout {
            keyWeight: 160
            readonly property real normalKeyWidth: normalKey.width
            readonly property real functionKeyWidth: mapFromItem(normalKey, normalKey.width / 2, 0).x
            KeyboardRow {
                Key {
                    key: Qt.Key_AsciiTilde
                    text: "~"
                }
                Key {
                    id: normalKey
                    key: Qt.Key_Agrave
                    text: "`"
                }
                Key {
                    key: Qt.Key_Bar
                    text: "|"
                }
                Key {
                    key: 0x7B
                    text: "·"
                }
                Key {
                    key: 0x221A
                    text: "√"
                }
                Key {
                    key: Qt.Key_division
                    text: "÷"
                }
                Key {
                    key: Qt.Key_multiply
                    text: "×"
                }
                Key {
                    key: Qt.Key_onehalf
                    text: "½"
                    alternativeKeys: "¼⅓½¾⅞"
                }
                Key {
                    key: Qt.Key_BraceLeft
                    text: "{"
                }
                Key {
                    key: Qt.Key_BraceRight
                    text: "}"
                }
            }
            KeyboardRow {
                Key {
                    key: Qt.Key_Dollar
                    text: "$"
                }
                Key {
                    key: 0x20AC
                    text: "€"
                }
                Key {
                    key: 0xC2
                    text: "£"
                }
                Key {
                    key: 0xA2
                    text: "¢"
                }
                Key {
                    key: 0xA5
                    text: "¥"
                }
                Key {
                    key: Qt.Key_AsciiCircum
                    text: "^"
                }
                Key {
                    key: Qt.Key_Equal
                    text: "="
                }
                Key {
                    key: Qt.Key_section
                    text: "§"
                }
                Key {
                    key: Qt.Key_BracketLeft
                    text: "["
                }
                Key {
                    key: Qt.Key_BracketRight
                    text: "]"
                }
            }
            KeyboardRow {
                Key {
                    displayText: "2/2"
                    functionKey: true
                    onClicked: secondPage = !secondPage
                    highlighted: true
                }
                Key {
                    key: 0x2122
                    text: '™'
                }
                Key {
                    key: 0x00AE
                    text: '®'
                }
                Key {
                    key: Qt.Key_guillemotleft
                    text: '«'
                }
                Key {
                    key: Qt.Key_guillemotright
                    text: '»'
                }
                Key {
                    key: Qt.Key_Semicolon
                    text: "\u1364"
                    alternativeKeys: ";"
                    smallText: ";"
                    smallTextVisible: true
                }
                Key {
                    key: 0x201C
                    text: '“'
                }
                Key {
                    key: 0x201D
                    text: '”'
                }
                Key {
                    key: Qt.Key_Backslash
                    text: "\\"
                }
                BackspaceKey {}
            }
            KeyboardRow {
                SymbolModeKey {
                    weight: functionKeyWidth
                    Layout.fillWidth: false
                    displayText: "\u1200\u1208\u1210"
                }
                ChangeLanguageKey {
                    weight: normalKeyWidth
                    Layout.fillWidth: false
                }
                Key {
                    key: Qt.Key_Comma
                    weight: normalKeyWidth
                    Layout.fillWidth: false
                    text: ","
                    smallText: "\u2699"
                    smallTextVisible: keyboard.isFunctionPopupListAvailable()
                    highlighted: true
                }
                SpaceKey {}
                Key {
                    key: 0x2026
                    weight: normalKeyWidth
                    Layout.fillWidth: false
                    text: "\u2026"
                    highlighted: true
                }
                HideKeyboardKey {
                    weight: normalKeyWidth
                    Layout.fillWidth: false
                }
                EnterKey {
                    weight: functionKeyWidth
                    Layout.fillWidth: false
                }
            }
        }
    }
}
