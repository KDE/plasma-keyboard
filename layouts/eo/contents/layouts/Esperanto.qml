// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    KeyboardRow {
        Key { text: "ŝ"; alternativeKeys: "ŝ1q"; smallText: "1" }
        Key { text: "ĝ"; alternativeKeys: "ĝ2w"; smallText: "2" }
        Key { text: "e"; alternativeKeys: "e3èé"; smallText: "3" }
        Key { text: "r"; alternativeKeys: "r4"; smallText: "4" }
        Key { text: "t"; alternativeKeys: "t5"; smallText: "5" }
        Key { text: "ŭ"; alternativeKeys: "ŭ6y"; smallText: "6" }
        Key { text: "u"; alternativeKeys: "u7ùú"; smallText: "7" }
        Key { text: "i"; alternativeKeys: "i8ìí"; smallText: "8" }
        Key { text: "o"; alternativeKeys: "o9òó"; smallText: "9" }
        Key { text: "p"; alternativeKeys: "p0"; smallText: "0" }
    }

    KeyboardRow {
        Key { text: "a"; alternativeKeys: "aàá" }
        Key { text: "s" }
        Key { text: "d" }
        Key { text: "f" }
        Key { text: "g" }
        Key { text: "h"; alternativeKeys: "ĥ" }
        Key { text: "j" }
        Key { text: "k" }
        Key { text: "l" }
        Key { text: "ĵ" }
    }

    KeyboardRow {
        ShiftKey { weight: root.defaultKeyWeight * 1.5 }
        Key { text: "z" }
        Key { text: "ĉ"; alternativeKeys: "x" }
        Key { text: "c" }
        Key { text: "v" }
        Key { text: "b" }
        Key { text: "n" }
        Key { text: "m" }
        BackspaceKey { weight: root.defaultKeyWeight * 1.5 }
    }

    KeyboardRow {
        SymbolModeKey { weight: root.defaultKeyWeight * 1.5 }
        ChangeLanguageKey {}
        Key { key: Qt.Key_Comma; text: ","; secondaryStyle: true }
        SpaceKey { weight: root.defaultKeyWeight * 3 }
        Key { key: Qt.Key_Period; text: "."; alternativeKeys: "!.?"; smallText: "!?"; secondaryStyle: true }
        HideKeyboardKey {}
        EnterKey { weight: root.defaultKeyWeight * 1.5 }
    }
}
