// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    KeyboardRow {
        Key { text: "q"; alternativeKeys: "q1"; smallText: "1" }
        Key { text: "w"; alternativeKeys: "w2"; smallText: "2" }
        Key { text: "e"; alternativeKeys: "e3"; smallText: "3" }
        Key { text: "r"; alternativeKeys: "r4"; smallText: "4" }
        Key { text: "t"; alternativeKeys: "t5"; smallText: "5" }
        Key { text: "y"; alternativeKeys: "y6"; smallText: "6" }
        Key { text: "u"; alternativeKeys: "u7üû"; smallText: "7" }
        Key { text: "ı"; alternativeKeys: "ı8"; smallText: "8" }
        Key { text: "o"; alternativeKeys: "o9ö"; smallText: "9" }
        Key { text: "p"; alternativeKeys: "p0"; smallText: "0" }
        Key { text: "ğ" }
        Key { text: "ü" }
    }

    KeyboardRow {
        FillerKey { weight: root.defaultKeyWeight * 0.5 }
        Key { text: "a"; alternativeKeys: "aâåä" }
        Key { text: "s"; alternativeKeys: "sşš" }
        Key { text: "d" }
        Key { text: "f" }
        Key { text: "g"; alternativeKeys: "gğ" }
        Key { text: "h" }
        Key { text: "j" }
        Key { text: "k" }
        Key { text: "l" }
        Key { text: "ş" }
        Key { text: "i"; displayText: uppercased ? "İ" : "i"; alternativeKeys: "iî" }
        FillerKey { weight: root.defaultKeyWeight * 0.5 }
    }

    KeyboardRow {
        ShiftKey { weight: root.defaultKeyWeight * 1.5 }
        Key { text: "z"; alternativeKeys: "zž" }
        Key { text: "x"; alternativeKeys: "$¢x₺€¥£" }
        Key { text: "c"; alternativeKeys: "cç" }
        Key { text: "v" }
        Key { text: "b" }
        Key { text: "n" }
        Key { text: "m" }
        Key { text: "ö" }
        Key { text: "ç" }
        BackspaceKey { weight: root.defaultKeyWeight * 1.5 }
    }

    KeyboardRow {
        SymbolModeKey { weight: root.defaultKeyWeight * 1.5 }
        ChangeLanguageKey {}
        Key { key: Qt.Key_Comma; text: ","; secondaryStyle: true }
        SpaceKey { weight: root.defaultKeyWeight * 5 }
        Key { key: Qt.Key_Period; text: "."; alternativeKeys: "!.?"; smallText: "!?"; secondaryStyle: true }
        HideKeyboardKey {}
        EnterKey { weight: root.defaultKeyWeight * 1.5 }
    }
}
