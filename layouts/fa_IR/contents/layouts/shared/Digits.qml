// SPDX-FileCopyrightText: 2016 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root
    textComposer: DirectTextComposer {}
    maxWidthToHeightRatio: 1

    KeyboardColumn {
        Layout.fillHeight: true
        Layout.alignment: Qt.AlignHCenter
        KeyboardRow { Key { text: "۷"; alternativeKeys: "۷7" } Key { text: "۸"; alternativeKeys: "۸8" } Key { text: "۹"; alternativeKeys: "۹9" } BackspaceKey {} }
        KeyboardRow { Key { text: "۴"; alternativeKeys: "۴4" } Key { text: "۵"; alternativeKeys: "۵5" } Key { text: "۶"; alternativeKeys: "۶6" } Key { text: " "; displayText: "\u2423"; repeat: true; showPreview: false; key: Qt.Key_Space; secondaryStyle: true } }
        KeyboardRow { Key { text: "۱"; alternativeKeys: "۱1" } Key { text: "۲"; alternativeKeys: "۲2" } Key { text: "۳"; alternativeKeys: "۳3" } HideKeyboardKey { visible: true } }
        KeyboardRow {
            ChangeLanguageKey { visible: true }
            Key { text: "۰"; alternativeKeys: "۰0" }
            Key {
                key: text === "," ? Qt.Key_Comma : Qt.Key_Period
                text: root.inputEngine ? Qt.locale(root.inputEngine.locale).decimalPoint : "٫"
            }
            EnterKey {}
        }
    }
}
