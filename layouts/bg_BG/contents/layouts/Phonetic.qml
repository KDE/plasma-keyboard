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
            Key { text: "я"; alternativeKeys: "я1"; smallText: "1" }
            Key { text: "в"; alternativeKeys: "в2"; smallText: "2" }
            Key { text: "е"; alternativeKeys: "е3"; smallText: "3" }
            Key { text: "р"; alternativeKeys: "р4"; smallText: "4" }
            Key { text: "т"; alternativeKeys: "т5"; smallText: "5" }
            Key { text: "ъ"; alternativeKeys: "ъ6"; smallText: "6" }
            Key { text: "у"; alternativeKeys: "у7"; smallText: "7" }
            Key { text: "и"; alternativeKeys: "и8ѝ"; smallText: "8" }
            Key { text: "о"; alternativeKeys: "о9"; smallText: "9" }
            Key { text: "п"; alternativeKeys: "п0"; smallText: "0" }
            Key { text: "ч" }
        }

        KeyboardRow {
            Key { text: "а" }
            Key { text: "с" }
            Key { text: "д" }
            Key { text: "ф" }
            Key { text: "г" }
            Key { text: "х" }
            Key { text: "й" }
            Key { text: "к" }
            Key { text: "л" }
            Key { text: "ш" }
            Key { text: "щ" }
        }

        KeyboardRow {
            ShiftKey { weight: root.defaultKeyWeight * 1.5 }
            Key { text: "з" }
            Key { text: "ь" }
            Key { text: "ц" }
            Key { text: "ж" }
            Key { text: "б" }
            Key { text: "н" }
            Key { text: "м" }
            Key { text: "ю" }
            BackspaceKey { weight: root.defaultKeyWeight * 1.5 }
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
