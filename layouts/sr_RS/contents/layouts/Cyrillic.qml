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
            Key { text: "љ"; alternativeKeys: "љ1"; smallText: "1" }
            Key { text: "њ"; alternativeKeys: "њ2"; smallText: "2" }
            Key { text: "е"; alternativeKeys: "е3"; smallText: "3" }
            Key { text: "р"; alternativeKeys: "р4"; smallText: "4" }
            Key { text: "т"; alternativeKeys: "т5"; smallText: "5" }
            Key { text: "з"; alternativeKeys: "з6ѕ"; smallText: "6" }
            Key { text: "у"; alternativeKeys: "у7"; smallText: "7" }
            Key { text: "и"; alternativeKeys: "и8"; smallText: "8" }
            Key { text: "о"; alternativeKeys: "о9"; smallText: "9" }
            Key { text: "п"; alternativeKeys: "п0"; smallText: "0" }
            Key { text: "ш" }
        }

        KeyboardRow {
            Key { text: "а" }
            Key { text: "с" }
            Key { text: "д" }
            Key { text: "ф" }
            Key { text: "г" }
            Key { text: "х" }
            Key { text: "ј" }
            Key { text: "к" }
            Key { text: "л" }
            Key { text: "ч" }
            Key { text: "ћ" }
        }

        KeyboardRow {
            ShiftKey { weight: root.defaultKeyWeight }
            Key { text: "џ" }
            Key { text: "ц" }
            Key { text: "в" }
            Key { text: "б" }
            Key { text: "н" }
            Key { text: "м" }
            Key { text: "ђ" }
            Key { text: "ж" }
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
}
