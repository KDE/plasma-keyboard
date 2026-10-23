// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard.components
import "shared" as Shared

KeyboardLayoutDefinition {
    symbols: Shared.Symbols {}
    numbers: Shared.Numbers {}
    digits: Shared.Digits {}
    dialpad: Shared.Dialpad {}

    KeyboardLayout {
        id: root

        KeyboardRow {
            Key { text: "q"; alternativeKeys: "q1"; smallText: "1" }
            Key { text: "w"; alternativeKeys: "w2"; smallText: "2" }
            Key { text: "e"; alternativeKeys: "e3éě"; smallText: "3" }
            Key { text: "r"; alternativeKeys: "r4ř"; smallText: "4" }
            Key { text: "t"; alternativeKeys: "t5ť"; smallText: "5" }
            Key { text: "z"; alternativeKeys: "z6ž"; smallText: "6" }
            Key { text: "u"; alternativeKeys: "u7úů"; smallText: "7" }
            Key { text: "i"; alternativeKeys: "i8í"; smallText: "8" }
            Key { text: "o"; alternativeKeys: "o9óö"; smallText: "9" }
            Key { text: "p"; alternativeKeys: "p0"; smallText: "0" }
        }

        KeyboardRow {
            FillerKey { weight: root.defaultKeyWeight * 0.5 }
            Key { text: "a"; alternativeKeys: "aáäå" }
            Key { text: "s"; alternativeKeys: "sš" }
            Key { text: "d"; alternativeKeys: "dď" }
            Key { text: "f" }
            Key { text: "g" }
            Key { text: "h" }
            Key { text: "j" }
            Key { text: "k" }
            Key { text: "l" }
            FillerKey { weight: root.defaultKeyWeight * 0.5 }
        }

        KeyboardRow {
            ShiftKey { weight: root.defaultKeyWeight * 1.5 }
            Key { text: "y"; alternativeKeys: "yý" }
            Key { text: "x" }
            Key { text: "c"; alternativeKeys: "cčć" }
            Key { text: "v" }
            Key { text: "b" }
            Key { text: "n"; alternativeKeys: "nňń" }
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
}
