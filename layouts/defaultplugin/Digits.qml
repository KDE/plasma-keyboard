// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    textComposer: DirectTextComposer {}
    maxWidthToHeightRatio: 1

    KeyboardColumn {
        Layout.fillHeight: true
        Layout.alignment: Qt.AlignHCenter
        KeyboardRow { Key { key: Qt.Key_7; text: "7" } Key { key: Qt.Key_8; text: "8" } Key { key: Qt.Key_9; text: "9" } BackspaceKey {} }
        KeyboardRow { Key { key: Qt.Key_4; text: "4" } Key { key: Qt.Key_5; text: "5" } Key { key: Qt.Key_6; text: "6" } Key { text: " "; displayText: "\u2423"; repeat: true; showPreview: false; key: Qt.Key_Space; secondaryStyle: true } }
        KeyboardRow { Key { key: Qt.Key_1; text: "1" } Key { key: Qt.Key_2; text: "2" } Key { key: Qt.Key_3; text: "3" } HideKeyboardKey { visible: true } }
        KeyboardRow {
            ChangeLanguageKey { visible: true }
            Key { key: Qt.Key_0; text: "0" }
            Key {
                key: text === "," ? Qt.Key_Comma : Qt.Key_Period
                text: inputEngine ? Qt.locale(inputEngine.locale).decimalPoint : "."
            }
            EnterKey {}
        }
    }
}
