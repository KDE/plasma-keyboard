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
            Key { text: ";"; alternativeKeys: ";1:"; smallText: "1" }
            Key { text: "ς"; alternativeKeys: "ς2"; smallText: "2" }
            Key { text: "ε"; alternativeKeys: "ε3έ"; smallText: "3" }
            Key { text: "ρ"; alternativeKeys: "ρ4"; smallText: "4" }
            Key { text: "τ"; alternativeKeys: "τ5"; smallText: "5" }
            Key { text: "υ"; alternativeKeys: "υ6ύϋΰ"; smallText: "6" }
            Key { text: "θ"; alternativeKeys: "θ7"; smallText: "7" }
            Key { text: "ι"; alternativeKeys: "ι8ίϊΐ"; smallText: "8" }
            Key { text: "ο"; alternativeKeys: "ο9ό"; smallText: "9" }
            Key { text: "π"; alternativeKeys: "π0"; smallText: "0" }
        }

        KeyboardRow {
            FillerKey { weight: root.defaultKeyWeight * 0.5 }
            Key { text: "α"; alternativeKeys: "αά" }
            Key { text: "σ" }
            Key { text: "δ" }
            Key { text: "φ" }
            Key { text: "γ" }
            Key { text: "η"; alternativeKeys: "ηή" }
            Key { text: "ξ" }
            Key { text: "κ" }
            Key { text: "λ" }
            FillerKey { weight: root.defaultKeyWeight * 0.5 }
        }

        KeyboardRow {
            ShiftKey { weight: root.defaultKeyWeight * 1.5 }
            Key { text: "ζ" }
            Key { text: "χ" }
            Key { text: "ψ" }
            Key { text: "ω"; alternativeKeys: "ωώ" }
            Key { text: "β" }
            Key { text: "ν" }
            Key { text: "μ" }
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
