// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
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
            KeyboardRow { Key { key: Qt.Key_7; text: "7" } Key { key: Qt.Key_8; text: "8" } Key { key: Qt.Key_9; text: "9" } BackspaceKey {} }
            KeyboardRow { Key { key: Qt.Key_4; text: "4" } Key { key: Qt.Key_5; text: "5" } Key { key: Qt.Key_6; text: "6" } Key { text: " "; displayText: "\u2423"; repeat: true; showPreview: false; key: Qt.Key_Space; secondaryStyle: true } }
            KeyboardRow { Key { key: Qt.Key_1; text: "1" } Key { key: Qt.Key_2; text: "2" } Key { key: Qt.Key_3; text: "3" } HideKeyboardKey { visible: true } }
            KeyboardRow {
                ChangeLanguageKey { visible: true }
                Key { key: Qt.Key_0; text: "0" }
                Key { key: Qt.Key_Period; text: "."; alternativeKeys: "., " }
                EnterKey {}
            }
        }
    }
}
