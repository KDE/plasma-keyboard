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
        Key { text: "ص"; alternativeKeys: "ص۱1ض"; smallText: "۱" }
        Key { text: "ق"; alternativeKeys: "ق۲2"; smallText: "۲" }
        Key { text: "ف"; alternativeKeys: "ف۳3"; smallText: "۳" }
        Key { text: "غ"; alternativeKeys: "غ۴4"; smallText: "۴" }
        Key { text: "ع"; alternativeKeys: "ع۵5"; smallText: "۵" }
        Key { text: "ه"; alternativeKeys: "ه۶6"; smallText: "۶" }
        Key { text: "خ"; alternativeKeys: "خ۷7"; smallText: "۷" }
        Key { text: "ح"; alternativeKeys: "ح۸8"; smallText: "۸" }
        Key { text: "ج"; alternativeKeys: "ج۹9"; smallText: "۹" }
        Key { text: "چ"; alternativeKeys: "چ۰0"; smallText: "۰" }
    }

    KeyboardRow {
        FillerKey { weight: root.defaultKeyWeight * 0.5 }
        Key { text: "ش" }
        Key { text: "س" }
        Key { text: "ی" }
        Key { text: "ب" }
        Key { text: "ل" }
        Key { text: "ا"; alternativeKeys: "\u0625\u0627\u0623\u0622" }
        Key { text: "ت"; alternativeKeys: "\u062A\u062B" }
        Key { text: "ن" }
        Key { text: "م" }
        Key { text: "ک" }
        FillerKey { weight: root.defaultKeyWeight * 0.5 }
    }

    KeyboardRow {
        Key { text: "‌"; alternativeKeys: "\u200C\u200D"; displayText: "\u25C2\u205E\u25B8"; displayAlternativeKeys: ["\u25B8\u205E\u25C2"] }
        Key { text: "َ"; alternativeKeys: "\u064C\u064E\u064F\u0652\u064B\u064D\u0640\u0651\u0650"; smallText: "◌ُ " }
        Key { text: "ط"; alternativeKeys: "\u0637\u0638" }
        Key { text: "ز"; alternativeKeys: "\u0632\u0698" }
        Key { text: "ر" }
        Key { text: "ذ" }
        Key { text: "د" }
        Key { text: "و"; alternativeKeys: "\u0624\u0648\u0621" }
        Key { text: "گ"; alternativeKeys: "\u06AF\u067E" }
        BackspaceKey {}
    }

    KeyboardRow {
        SymbolModeKey { weight: root.defaultKeyWeight * 1.5; displayText: "۱۲۳؟" }
        ChangeLanguageKey {}
        Key { text: "،"; secondaryStyle: true }
        SpaceKey { weight: root.defaultKeyWeight * 5 }
        Key { text: "."; alternativeKeys: "!,،؛.؟"; secondaryStyle: true }
        HideKeyboardKey {}
        EnterKey { weight: root.defaultKeyWeight * 1.5 }
    }
}
