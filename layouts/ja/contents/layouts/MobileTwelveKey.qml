// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    textComposer: AnthyTextComposer { inputMode: "hiragana" }
    sharedLayouts: ["symbols"]
    readonly property bool latinMode: textComposer.inputMode === "latin"

    KeyboardRow {
        visible: !latinMode
        InputModeKey {
            textComposer: root.textComposer
            showNextMode: true
            modes: [
                { value: "hiragana", label: "あ" },
                { value: "latin", label: "ABC" }
            ]
        }
        FlickKey { text: "\u3042"; baseCommitText: "a"; alternativeKeys: "\u3042\u3044\u3046\u3048\u304A"; alternativeCommitTexts: ["i", "u", "e", "o"]; tapSequenceEnabled: true }
        FlickKey { text: "\u304b"; baseCommitText: "ka"; alternativeKeys: "\u304b\u304d\u304f\u3051\u3053"; alternativeCommitTexts: ["ki", "ku", "ke", "ko"]; tapSequenceEnabled: true }
        FlickKey { text: "\u3055"; baseCommitText: "sa"; alternativeKeys: "\u3055\u3057\u3059\u305b\u305d"; alternativeCommitTexts: ["shi", "su", "se", "so"]; tapSequenceEnabled: true }
        BackspaceKey {}
    }

    KeyboardRow {
        visible: !latinMode
        Key { key: Qt.Key_Left; displayText: "\u2190"; repeat: true; noModifier: true; functionKey: true; secondaryStyle: true }
        FlickKey { text: "\u305f"; baseCommitText: "ta"; alternativeKeys: "\u305f\u3061\u3064\u3066\u3068"; alternativeCommitTexts: ["chi", "tsu", "te", "to"]; tapSequenceEnabled: true }
        FlickKey { text: "\u306a"; baseCommitText: "na"; alternativeKeys: "\u306a\u306b\u306c\u306d\u306e"; alternativeCommitTexts: ["ni", "nu", "ne", "no"]; tapSequenceEnabled: true }
        FlickKey { text: "\u306f"; baseCommitText: "ha"; alternativeKeys: "\u306f\u3072\u3075\u3078\u307b"; alternativeCommitTexts: ["hi", "fu", "he", "ho"]; tapSequenceEnabled: true }
        Key { key: Qt.Key_Right; displayText: "\u2192"; repeat: true; noModifier: true; functionKey: true; secondaryStyle: true }
    }

    KeyboardRow {
        visible: !latinMode
        SymbolModeKey {}
        FlickKey { text: "\u307e"; baseCommitText: "ma"; alternativeKeys: "\u307e\u307f\u3080\u3081\u3082"; alternativeCommitTexts: ["mi", "mu", "me", "mo"]; tapSequenceEnabled: true }
        FlickKey { text: "\u3084"; baseCommitText: "ya"; alternativeKeys: "\u3084\uff08\u3086\uff09\u3088"; alternativeCommitTexts: ["\uff08", "yu", "\uff09", "yo"]; tapSequenceEnabled: true; tapSequenceCommitTexts: ["ya", "yu", "yo"] }
        FlickKey { text: "\u3089"; baseCommitText: "ra"; alternativeKeys: "\u3089\u308a\u308b\u308c\u308d"; alternativeCommitTexts: ["ri", "ru", "re", "ro"]; tapSequenceEnabled: true }
        SpaceKey { displayText: "空白" }
    }

    KeyboardRow {
        visible: !latinMode
        ChangeLanguageKey { weight: defaultKeyWeight / 2 }
        HideKeyboardKey { weight: defaultKeyWeight / 2 }
        FlickKey { text: "\u3099"; baseCommitText: "\u3099"; alternativeKeys: "\u3099\u309a\u5c0f"; alternativeCommitTexts: ["\u309a", "small"]; tapSequenceEnabled: true }
        FlickKey { text: "\u308f"; baseCommitText: "wa"; alternativeKeys: "\u308f\u3092\u3093\u30fc\u301c"; alternativeCommitTexts: ["wo", "nn", "-", "~"]; tapSequenceEnabled: true }
        FlickKey { text: "\u3001"; baseCommitText: "\u3001"; alternativeKeys: "\u3001\u3002?!\u2026"; alternativeCommitTexts: ["\u3002", "?", "!", "\u2026"] }
        EnterKey {}
    }

    KeyboardRow {
        visible: latinMode
        InputModeKey {
            textComposer: root.textComposer
            showNextMode: true
            modes: [
                { value: "hiragana", label: "あ" },
                { value: "latin", label: "ABC" }
            ]
        }
        FlickKey { displayText: "@#\\&"; text: "@"; baseCommitText: "@"; alternativeKeys: "#\\&1"; alternativeCommitTexts: ["#", "\\", "&", "1"]; tapSequenceEnabled: true }
        FlickKey { displayText: "abc"; text: "a"; baseCommitText: "a"; alternativeKeys: "bc2"; alternativeCommitTexts: ["b", "c", "2"]; tapSequenceEnabled: true }
        FlickKey { displayText: "def"; text: "d"; baseCommitText: "d"; alternativeKeys: "ef3"; alternativeCommitTexts: ["e", "f", "3"]; tapSequenceEnabled: true }
        BackspaceKey {}
    }

    KeyboardRow {
        visible: latinMode
        Key { key: Qt.Key_Left; displayText: "\u2190"; repeat: true; noModifier: true; functionKey: true; secondaryStyle: true }
        FlickKey { displayText: "ghi"; text: "g"; baseCommitText: "g"; alternativeKeys: "hi4"; alternativeCommitTexts: ["h", "i", "4"]; tapSequenceEnabled: true }
        FlickKey { displayText: "jkl"; text: "j"; baseCommitText: "j"; alternativeKeys: "kl5"; alternativeCommitTexts: ["k", "l", "5"]; tapSequenceEnabled: true }
        FlickKey { displayText: "mno"; text: "m"; baseCommitText: "m"; alternativeKeys: "no6"; alternativeCommitTexts: ["n", "o", "6"]; tapSequenceEnabled: true }
        Key { key: Qt.Key_Right; displayText: "\u2192"; repeat: true; noModifier: true; functionKey: true; secondaryStyle: true }
    }

    KeyboardRow {
        visible: latinMode
        SymbolModeKey {}
        FlickKey { displayText: "pqrs"; text: "p"; baseCommitText: "p"; alternativeKeys: "qrs7"; alternativeCommitTexts: ["q", "r", "s", "7"]; tapSequenceEnabled: true }
        FlickKey { displayText: "tuv"; text: "t"; baseCommitText: "t"; alternativeKeys: "uv8"; alternativeCommitTexts: ["u", "v", "8"]; tapSequenceEnabled: true }
        FlickKey { displayText: "wxyz"; text: "w"; baseCommitText: "w"; alternativeKeys: "xyz9"; alternativeCommitTexts: ["x", "y", "z", "9"]; tapSequenceEnabled: true }
        SpaceKey { displayText: "空白" }
    }

    KeyboardRow {
        visible: latinMode
        ChangeLanguageKey { weight: defaultKeyWeight / 2 }
        HideKeyboardKey { weight: defaultKeyWeight / 2 }
        ShiftKey {}
        FlickKey { displayText: "'\"()"; text: "'"; baseCommitText: "'"; alternativeKeys: "\"()0"; alternativeCommitTexts: ["\"", "(", ")", "0"]; tapSequenceEnabled: true }
        FlickKey { displayText: ".,?!"; text: "."; baseCommitText: "."; alternativeKeys: ",?!."; alternativeCommitTexts: [",", "?", "!", "."]; tapSequenceEnabled: true }
        EnterKey {}
    }
}
