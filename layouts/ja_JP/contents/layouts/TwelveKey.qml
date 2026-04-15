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
        FlickKey { text: "あ"; baseCommitText: "a"; alternativeKeys: "あいうえお"; alternativeCommitTexts: ["i", "u", "e", "o"]; tapSequenceEnabled: true }
        FlickKey { text: "か"; baseCommitText: "ka"; alternativeKeys: "かきくけこ"; alternativeCommitTexts: ["ki", "ku", "ke", "ko"]; tapSequenceEnabled: true }
        FlickKey { text: "さ"; baseCommitText: "sa"; alternativeKeys: "さしすせそ"; alternativeCommitTexts: ["shi", "su", "se", "so"]; tapSequenceEnabled: true }
        BackspaceKey {}
    }

    KeyboardRow {
        visible: !latinMode
        Key { key: Qt.Key_Left; displayText: "←"; repeat: true; noModifier: true; functionKey: true; secondaryStyle: true }
        FlickKey { text: "た"; baseCommitText: "ta"; alternativeKeys: "たちつてと"; alternativeCommitTexts: ["chi", "tsu", "te", "to"]; tapSequenceEnabled: true }
        FlickKey { text: "な"; baseCommitText: "na"; alternativeKeys: "なにぬねの"; alternativeCommitTexts: ["ni", "nu", "ne", "no"]; tapSequenceEnabled: true }
        FlickKey { text: "は"; baseCommitText: "ha"; alternativeKeys: "はひふへほ"; alternativeCommitTexts: ["hi", "fu", "he", "ho"]; tapSequenceEnabled: true }
        Key { key: Qt.Key_Right; displayText: "→"; repeat: true; noModifier: true; functionKey: true; secondaryStyle: true }
    }

    KeyboardRow {
        visible: !latinMode
        SymbolModeKey {}
        FlickKey { text: "ま"; baseCommitText: "ma"; alternativeKeys: "まみむめも"; alternativeCommitTexts: ["mi", "mu", "me", "mo"]; tapSequenceEnabled: true }
        FlickKey { text: "や"; baseCommitText: "ya"; alternativeKeys: "や（ゆ）よ"; alternativeCommitTexts: ["（", "yu", "）", "yo"]; tapSequenceEnabled: true; tapSequenceCommitTexts: ["ya", "yu", "yo"] }
        FlickKey { text: "ら"; baseCommitText: "ra"; alternativeKeys: "らりるれろ"; alternativeCommitTexts: ["ri", "ru", "re", "ro"]; tapSequenceEnabled: true }
        SpaceKey { iconName: "keyboard-spacebar" }
    }

    KeyboardRow {
        visible: !latinMode
        ChangeLanguageKey { weight: defaultKeyWeight / 2 }
        HideKeyboardKey { weight: defaultKeyWeight / 2 }
        Key {
            text: "小⇔大" // Small 小 and big 大
            smallText: "゜゛" // Handakuten ゜ and dakuten ゛
            commitText: ""
            onClicked: root.textComposer.cycleKanaModifier()
        }
        FlickKey { text: "わ"; baseCommitText: "wa"; alternativeKeys: "わをんー〜"; alternativeCommitTexts: ["wo", "nn", "-", "~"]; tapSequenceEnabled: true }
        FlickKey { text: "、"; baseCommitText: "、"; alternativeKeys: "、。？！…"; alternativeCommitTexts: ["。", "？", "！", "…"] }
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
        Key { key: Qt.Key_Left; displayText: "←"; repeat: true; noModifier: true; functionKey: true; secondaryStyle: true }
        FlickKey { displayText: "ghi"; text: "g"; baseCommitText: "g"; alternativeKeys: "hi4"; alternativeCommitTexts: ["h", "i", "4"]; tapSequenceEnabled: true }
        FlickKey { displayText: "jkl"; text: "j"; baseCommitText: "j"; alternativeKeys: "kl5"; alternativeCommitTexts: ["k", "l", "5"]; tapSequenceEnabled: true }
        FlickKey { displayText: "mno"; text: "m"; baseCommitText: "m"; alternativeKeys: "no6"; alternativeCommitTexts: ["n", "o", "6"]; tapSequenceEnabled: true }
        Key { key: Qt.Key_Right; displayText: "→"; repeat: true; noModifier: true; functionKey: true; secondaryStyle: true }
    }

    KeyboardRow {
        visible: latinMode
        SymbolModeKey {}
        FlickKey { displayText: "pqrs"; text: "p"; baseCommitText: "p"; alternativeKeys: "qrs7"; alternativeCommitTexts: ["q", "r", "s", "7"]; tapSequenceEnabled: true }
        FlickKey { displayText: "tuv"; text: "t"; baseCommitText: "t"; alternativeKeys: "uv8"; alternativeCommitTexts: ["u", "v", "8"]; tapSequenceEnabled: true }
        FlickKey { displayText: "wxyz"; text: "w"; baseCommitText: "w"; alternativeKeys: "xyz9"; alternativeCommitTexts: ["x", "y", "z", "9"]; tapSequenceEnabled: true }
        SpaceKey { iconName: "keyboard-spacebar" }
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
