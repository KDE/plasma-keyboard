// SPDX-FileCopyrightText: 2016 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root
    textComposer: DirectTextComposer {}
    maxWidthToHeightRatio: 15 / 8

    KeyboardRow {
        KeyboardColumn {
            weight: 300
            KeyboardRow { Key { key: Qt.Key_ParenLeft; text: "(" } Key { key: Qt.Key_ParenRight; text: ")" } Key { key: Qt.Key_Comma; text: "," } }
            KeyboardRow { Key { key: Qt.Key_division; text: "\u00F7" } Key { key: Qt.Key_multiply; text: "\u00D7" } Key { key: Qt.Key_Plus; text: "+" } }
            KeyboardRow { Key { key: Qt.Key_AsciiCircum; text: "^" } Key { key: Qt.Key_Slash; text: "/" } Key { key: Qt.Key_Minus; text: "-" } }
            KeyboardRow { Key { key: 0x221A; text: "√" } Key { key: Qt.Key_Percent; text: "%" } Key { key: Qt.Key_Asterisk; text: "*" } }
        }
        KeyboardColumn {
            weight: 50
            KeyboardRow { FillerKey {} }
        }
        KeyboardColumn {
            weight: 400
            KeyboardRow { Key { text: "۷"; alternativeKeys: "۷7" } Key { text: "۸"; alternativeKeys: "۸8" } Key { text: "۹"; alternativeKeys: "۹9" } BackspaceKey {} }
            KeyboardRow { Key { text: "۴"; alternativeKeys: "۴4" } Key { text: "۵"; alternativeKeys: "۵5" } Key { text: "۶"; alternativeKeys: "۶6" } Key { text: " "; displayText: "\u2423"; repeat: true; showPreview: false; key: Qt.Key_Space; secondaryStyle: true } }
            KeyboardRow { Key { text: "۱"; alternativeKeys: "۱1" } Key { text: "۲"; alternativeKeys: "۲2" } Key { text: "۳"; alternativeKeys: "۳3" } HideKeyboardKey { visible: true } }
            KeyboardRow {
                ChangeLanguageKey { visible: true }
                Key { text: "۰"; alternativeKeys: "۰0" }
                Key { text: root.inputEngine ? Qt.locale(root.inputEngine.locale).decimalPoint : "٫"; alternativeKeys: "٫.," }
                EnterKey {}
            }
        }
    }
}
