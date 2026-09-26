/*
    SPDX-FileCopyrightText: 2026 Abenezer Wesenseged <wseged@proton.me>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    textComposer: AmharicTextComposer {}

    KeyboardRow {
        Key { text: "ሀ" }
        Key { text: "ለ" }
        Key { text: "ሐ" }
        Key { text: "መ" }
        Key { text: "ሠ" }
        Key { text: "ረ" }
        Key { text: "ሰ" }
        Key { text: "ሸ" }
        Key { text: "ቀ"; alternativeKeys: "\u1248\u124A\u124B\u124C\u124D"; smallText: "ቈ" }
        Key { text: "በ" }
        Key { text: "ቨ" }
        Key { text: "ተ" }
    }

    KeyboardRow {
        FillerKey { weight: root.defaultKeyWeight * 0.5 }
        Key { text: "ቸ" }
        Key { text: "ኀ"; alternativeKeys: "\u1288\u128A\u128B\u128C\u128D"; smallText: "ኈ" }
        Key { text: "ነ" }
        Key { text: "ኘ" }
        Key { text: "አ" }
        Key { text: "ከ"; alternativeKeys: "\u12B0\u12B2\u12B3\u12B4\u12B5"; smallText: "ኰ" }
        Key { text: "ኸ"; alternativeKeys: "\u12C0\u12C2\u12C3\u12C4\u12C5"; smallText: "ዀ" }
        Key { text: "ወ" }
        Key { text: "ዐ" }
        Key { text: "ዘ" }
        Key { text: "ዠ" }
        FillerKey { weight: root.defaultKeyWeight * 0.5 }
    }

    KeyboardRow {
        Key { text: "የ" }
        Key { text: "ደ" }
        Key { text: "ጀ" }
        Key { text: "ገ"; alternativeKeys: "\u1310\u1312\u1313\u1314\u1315"; smallText: "ጐ" }
        Key { text: "ጠ" }
        Key { text: "ጨ" }
        Key { text: "ጰ" }
        Key { text: "ጸ" }
        Key { text: "ፀ" }
        Key { text: "ፈ" }
        Key { text: "ፐ" }
        BackspaceKey {}
    }

    KeyboardRow {
        SymbolModeKey { weight: root.defaultKeyWeight * 1.5; displayText: "፩፪፫" }
        ChangeLanguageKey {}
        Key { text: "፣"; secondaryStyle: true }
        SpaceKey { weight: root.defaultKeyWeight * 5 }
        Key { text: "።"; alternativeKeys: "።፡፤!?"; secondaryStyle: true }
        HideKeyboardKey {}
        EnterKey { weight: root.defaultKeyWeight * 1.5 }
    }
}
