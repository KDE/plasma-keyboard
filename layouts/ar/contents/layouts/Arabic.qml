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
        Key { text: "ض"; alternativeKeys: "ض١1"; smallText: "١" }
        Key { text: "ص"; alternativeKeys: "ص٢2"; smallText: "٢" }
        Key { text: "ث"; alternativeKeys: "ث٣3"; smallText: "٣" }
        Key { text: "ق"; alternativeKeys: "ق٤4"; smallText: "٤" }
        Key { text: "ف"; alternativeKeys: "ف٥5"; smallText: "٥" }
        Key { text: "غ"; alternativeKeys: "غ٦6"; smallText: "٦" }
        Key { text: "ع"; alternativeKeys: "ع٧7"; smallText: "٧" }
        Key { text: "ه"; alternativeKeys: "ه٨8"; smallText: "٨" }
        Key { text: "خ"; alternativeKeys: "خ٩9"; smallText: "٩" }
        Key { text: "ح"; alternativeKeys: "ح٠0"; smallText: "٠" }
        Key { text: "ج"; alternativeKeys: "\u062C\u0686" }
        Key { text: "د"; alternativeKeys: "\u062F\uFDFC" }
    }

    KeyboardRow {
        FillerKey { weight: root.defaultKeyWeight * 0.5 }
        Key { text: "ش" }
        Key { text: "س" }
        Key { text: "ي"; alternativeKeys: "يئى" }
        Key { text: "ب"; alternativeKeys: "\u0628\u067E" }
        Key { text: "ل"; alternativeKeys: ["\u0644\u0623", "\u0644\u0622", "\u0644", "\u0644\u0627", "\u0644\u0625"] }
        Key { text: "ا"; alternativeKeys: "\u0625\u0627\u0623\u0622\u0671" }
        Key { text: "ت" }
        Key { text: "ن" }
        Key { text: "م" }
        Key { text: "ك"; alternativeKeys: "\u0643\u06AF" }
        Key { text: "ذ" }
        FillerKey { weight: root.defaultKeyWeight * 0.5 }
    }

    KeyboardRow {
        Key { text: "ئ" }
        Key { text: "ء" }
        Key { text: "ؤ" }
        Key { text: "ر" }
        Key { text: "ى"; alternativeKeys: "ىئ" }
        Key { text: "ة" }
        Key { text: "و"; alternativeKeys: "وؤ" }
        Key { text: "ز" }
        Key { text: "ط" }
        Key { text: "ظ"; alternativeKeys: "\u064E\u064C\u0650\u064D\u0651\u0652\u0670\u0653\u0640\u0638\u064B\u064F"; smallText: "◌ِ " }
        BackspaceKey {}
    }

    KeyboardRow {
        SymbolModeKey { weight: root.defaultKeyWeight * 1.5; displayText: "١٢٣؟" }
        ChangeLanguageKey {}
        Key { text: "،"; alternativeKeys: "،٬,"; secondaryStyle: true }
        SpaceKey { weight: root.defaultKeyWeight * 5 }
        Key { text: "."; alternativeKeys: "!,،؛.؟"; secondaryStyle: true }
        HideKeyboardKey {}
        EnterKey { weight: root.defaultKeyWeight * 1.5 }
    }
}
