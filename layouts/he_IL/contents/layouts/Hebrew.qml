// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components
import "shared" as Shared

KeyboardLayoutDefinition {
    textComposer: DirectTextComposer {}

    symbols: Shared.Symbols {}
    numbers: Shared.Numbers {}
    digits: Shared.Digits {}
    dialpad: Shared.Dialpad {}

    KeyboardLayout {
        id: root

        KeyboardRow {
            Key { key: Qt.Key_Apostrophe; text: "'"; alternativeKeys: "'\"" }
            Key { text: "ק"; alternativeKeys: "\u05B3\u05E7\u05B8\u05BB" }
            Key { text: "ר" }
            Key { text: "א" }
            Key { text: "ט" }
            Key { text: "ו"; alternativeKeys: [ "\u05D5", "\u05D5\u05B9", "\u05D5\u05BC" ] }
            Key { text: "ן" }
            Key { text: "ם" }
            Key { text: "פ"; alternativeKeys: "\u05B2\u05B7\u05E4" }
        }

        KeyboardRow {
            FillerKey { weight: root.defaultKeyWeight * 0.5 }
            Key { text: "ש"; alternativeKeys: [ "\u05E9", "\u05B0", "\u05E9\u05C2", "\u05E9\u05C1" ] }
            Key { text: "ד"; alternativeKeys: "\u05D3\u05BC" }
            Key { text: "ג"; alternativeKeys: [ "\u05D2", "\u05D2\u05F3" ] }
            Key { text: "כ" }
            Key { text: "ע" }
            Key { text: "י"; alternativeKeys: [ "\u05D9", "\u05F2\u05B7" ] }
            Key { text: "ח"; alternativeKeys: [ "\u05B9", "\u05D7", "\u05D7\u05F3", "\u05B4" ] }
            Key { text: "ל" }
            Key { text: "ך" }
            Key { text: "ף" }
            FillerKey { weight: root.defaultKeyWeight * 0.5 }
        }

        KeyboardRow {
            Key { text: "ז"; alternativeKeys: [ "\u05D6", "\u05D6\u05F3" ] }
            Key { text: "ס"; alternativeKeys: "\u05E1\u05B6\u05B1" }
            Key { text: "ב" }
            Key { text: "ה" }
            Key { text: "נ" }
            Key { text: "מ" }
            Key { text: "צ"; alternativeKeys: [ "\u05E6", "\u05E6\u05F3", "\u05B5" ] }
            Key { text: "ת"; alternativeKeys: [ "\u05EA", "\u05EA\u05F3" ] }
            Key { text: "ץ"; alternativeKeys: [ "\u05E5", "\u05E5\u05F3" ] }
            BackspaceKey {}
        }

        KeyboardRow {
            SymbolModeKey { weight: root.defaultKeyWeight * 1.5 }
            ChangeLanguageKey {}
            Key { text: ","; secondaryStyle: true }
            SpaceKey { weight: root.defaultKeyWeight * 5 }
            Key { text: "."; alternativeKeys: "!.?"; secondaryStyle: true }
            HideKeyboardKey {}
            EnterKey { weight: root.defaultKeyWeight * 1.5 }
        }
    }
}
