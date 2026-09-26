// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    KeyboardRow {
        Key { text: "й"; alternativeKeys: "й1"; smallText: "1" }
        Key { text: "ц"; alternativeKeys: "ц2"; smallText: "2" }
        Key { text: "у"; alternativeKeys: "у3"; smallText: "3" }
        Key { text: "к"; alternativeKeys: "к4"; smallText: "4" }
        Key { text: "е"; alternativeKeys: "е5ё"; smallText: "5" }
        Key { text: "н"; alternativeKeys: "н6"; smallText: "6" }
        Key { text: "г"; alternativeKeys: "г7"; smallText: "7" }
        Key { text: "ш"; alternativeKeys: "ш8"; smallText: "8" }
        Key { text: "щ"; alternativeKeys: "щ9"; smallText: "9" }
        Key { text: "з"; alternativeKeys: "з0"; smallText: "0" }
        Key { text: "х" }
    }

    KeyboardRow {
        Key { text: "ф" }
        Key { text: "ы" }
        Key { text: "в" }
        Key { text: "а" }
        Key { text: "п" }
        Key { text: "р" }
        Key { text: "о" }
        Key { text: "л" }
        Key { text: "д" }
        Key { text: "ж" }
        Key { text: "э" }
    }

    KeyboardRow {
        ShiftKey { weight: root.defaultKeyWeight }
        Key { text: "я" }
        Key { text: "ч" }
        Key { text: "с" }
        Key { text: "м" }
        Key { text: "и" }
        Key { text: "т" }
        Key { text: "ь"; alternativeKeys: "ьъ" }
        Key { text: "б" }
        Key { text: "ю" }
        BackspaceKey { weight: root.defaultKeyWeight }
    }

    KeyboardRow {
        SymbolModeKey { weight: root.defaultKeyWeight * 1.5 }
        ChangeLanguageKey {}
        Key { key: Qt.Key_Comma; text: ","; secondaryStyle: true }
        SpaceKey { weight: root.defaultKeyWeight * 4 }
        Key { key: Qt.Key_Period; text: "."; alternativeKeys: "!.?"; smallText: "!?"; secondaryStyle: true }
        HideKeyboardKey {}
        EnterKey { weight: root.defaultKeyWeight * 1.5 }
    }
}
