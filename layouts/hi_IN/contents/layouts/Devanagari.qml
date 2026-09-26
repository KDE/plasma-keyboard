// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    textComposer: DirectTextComposer {}

    KeyboardRow {
        Key { text: "औ"; alternativeKeys: "औ१ौ"; smallText: "१" }
        Key { text: "ऐ"; alternativeKeys: "ऐ२ै"; smallText: "२" }
        Key { text: "आ"; alternativeKeys: "आ३ा"; smallText: "३" }
        Key { text: "ई"; alternativeKeys: "ई४ी"; smallText: "४" }
        Key { text: "ऊ"; alternativeKeys: "ऊ५ू"; smallText: "५" }
        Key { text: "ब"; alternativeKeys: "ब६भ"; smallText: "६" }
        Key { text: "ह"; alternativeKeys: "ह७"; smallText: "७" }
        Key { text: "ग"; alternativeKeys: ["ग", "८", "घ", "ग़"]; smallText: "८" }
        Key { text: "द"; alternativeKeys: "द९ध"; smallText: "९" }
        Key { text: "ज"; alternativeKeys: ["ज", "०", "ञ", "झ", "ज़", "ज्ञ"]; smallText: "०" }
        Key { text: "ड"; alternativeKeys: ["ड", "ढ", "ड़", "ढ़"] }
    }

    KeyboardRow {
        Key { text: "ओ"; alternativeKeys: "\u0913\u094B" }
        Key { text: "ए"; alternativeKeys: "\u090F\u0947" }
        Key { text: "अ"; alternativeKeys: "अ्" }
        Key { text: "इ"; alternativeKeys: "\u0907\u093F" }
        Key { text: "उ"; alternativeKeys: "\u0909\u0941" }
        Key { text: "प"; alternativeKeys: ["प", "फ", "फ़"] }
        Key { text: "र"; alternativeKeys: "\u0930\u090B\u0943" }
        Key { text: "क"; alternativeKeys: ["क", "ख", "क़", "ख़", "क्ष"] }
        Key { text: "त"; alternativeKeys: ["\u0924", "\u0925", "\u0924\u094D\u0930"] }
        Key { text: "च"; alternativeKeys: "\u091A\u091B" }
        Key { text: "ट"; alternativeKeys: "\u091F\u0920" }
    }

    KeyboardRow {
        Key { text: "‌"; alternativeKeys: "\u200C\u200D"; displayText: "\u25C2\u205E\u25B8"; displayAlternativeKeys: ["\u25B8\u205E\u25C2"] }
        Key { text: "ऑ"; alternativeKeys: "\u0911\u0949" }
        Key { text: "्"; alternativeKeys: "\u094D\u0945\u090D" }
        Key { text: "ं"; alternativeKeys: "\u093C\u0902\u0903\u0901\u093D" }
        Key { text: "म"; alternativeKeys: "\u092E\u0950" }
        Key { text: "न"; alternativeKeys: "\u0919\u0928\u0923\u091E" }
        Key { text: "व" }
        Key { text: "ल"; alternativeKeys: "\u0932\u0933" }
        Key { text: "स"; alternativeKeys: ["\u0936\u094D\u0930", "\u0938", "\u0936", "\u0937"] }
        Key { text: "य"; alternativeKeys: "\u0943" }
        BackspaceKey {}
    }

    KeyboardRow {
        SymbolModeKey { weight: root.defaultKeyWeight * 1.5; displayText: "&१२३" }
        ChangeLanguageKey {}
        Key { text: ","; secondaryStyle: true }
        SpaceKey { weight: root.defaultKeyWeight * 5 }
        Key { text: "।"; alternativeKeys: "।.॥!?"; secondaryStyle: true }
        HideKeyboardKey {}
        EnterKey { weight: root.defaultKeyWeight * 1.5 }
    }
}
