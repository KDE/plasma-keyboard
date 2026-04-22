// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayoutLoader {
    id: root

    sourceComponent: inputEngine && inputEngine.uppercase ? page2 : page1
    Component {
        id: page1

        KeyboardLayout {
            id: page1Layout

            textComposer: HangulTextComposer {}
            sharedLayouts: ["symbols"]
            readonly property real functionKeyWeight: defaultKeyWeight * 1.5

            KeyboardRow {
                Key { text: "\u3142"; alternativeKeys: "\u3142\u31431"; smallText: "1" }
                Key { text: "\u3148"; alternativeKeys: "\u3148\u31492"; smallText: "2" }
                Key { text: "\u3137"; alternativeKeys: "\u3137\u31383"; smallText: "3" }
                Key { text: "\u3131"; alternativeKeys: "\u3131\u31324"; smallText: "4" }
                Key { text: "\u3145"; alternativeKeys: "\u3145\u31465"; smallText: "5" }
                Key { text: "\u315B"; alternativeKeys: "\u315B6"; smallText: "6" }
                Key { text: "\u3155"; alternativeKeys: "\u31557"; smallText: "7" }
                Key { text: "\u3151"; alternativeKeys: "\u31518"; smallText: "8" }
                Key { text: "\u3150"; alternativeKeys: "\u3150\u31529"; smallText: "9" }
                Key { text: "\u3154"; alternativeKeys: "\u3154\u31560"; smallText: "0" }
            }

            KeyboardRow {
                FillerKey { weight: defaultKeyWeight / 2 }
                Key { text: "\u3141" }
                Key { text: "\u3134" }
                Key { text: "\u3147" }
                Key { text: "\u3139" }
                Key { text: "\u314E" }
                Key { text: "\u3157" }
                Key { text: "\u3153" }
                Key { text: "\u314F" }
                Key { text: "\u3163" }
                FillerKey { weight: defaultKeyWeight / 2 }
            }

            KeyboardRow {
                ShiftKey { weight: functionKeyWeight }
                Key { text: "\u314B" }
                Key { text: "\u314C" }
                Key { text: "\u314A" }
                Key { text: "\u314D" }
                Key { text: "\u3160" }
                Key { text: "\u315C" }
                Key { text: "\u3161" }
                BackspaceKey { weight: functionKeyWeight }
            }

            KeyboardRow {
                SymbolModeKey { weight: functionKeyWeight }
                ChangeLanguageKey { weight: defaultKeyWeight }
                Key { key: Qt.Key_Comma; weight: defaultKeyWeight; text: ","; secondaryStyle: true }
                SpaceKey { weight: 300 }
                Key { key: Qt.Key_Period; weight: defaultKeyWeight; text: "."; alternativeKeys: "!.?"; smallText: "!?"; secondaryStyle: true }
                HideKeyboardKey { weight: defaultKeyWeight }
                EnterKey { weight: functionKeyWeight }
            }
        }
    }

    Component {
        id: page2

        KeyboardLayout {
            textComposer: HangulTextComposer {}
            sharedLayouts: ["symbols"]
            readonly property real functionKeyWeight: defaultKeyWeight * 1.5

            KeyboardRow {
                Key { text: "\u3143"; alternativeKeys: "\u31431"; smallText: "1" }
                Key { text: "\u3149"; alternativeKeys: "\u31492"; smallText: "2" }
                Key { text: "\u3138"; alternativeKeys: "\u31383"; smallText: "3" }
                Key { text: "\u3132"; alternativeKeys: "\u31324"; smallText: "4" }
                Key { text: "\u3146"; alternativeKeys: "\u31465"; smallText: "5" }
                Key { text: "\u315B"; alternativeKeys: "\u315B6"; smallText: "6" }
                Key { text: "\u3155"; alternativeKeys: "\u31557"; smallText: "7" }
                Key { text: "\u3151"; alternativeKeys: "\u31518"; smallText: "8" }
                Key { text: "\u3152"; alternativeKeys: "\u31529"; smallText: "9" }
                Key { text: "\u3156"; alternativeKeys: "\u31560"; smallText: "0" }
            }

            KeyboardRow {
                FillerKey { weight: defaultKeyWeight / 2 }
                Key { text: "\u3141" }
                Key { text: "\u3134" }
                Key { text: "\u3147" }
                Key { text: "\u3139" }
                Key { text: "\u314E" }
                Key { text: "\u3157" }
                Key { text: "\u3153" }
                Key { text: "\u314F" }
                Key { text: "\u3163" }
                FillerKey { weight: defaultKeyWeight / 2 }
            }

            KeyboardRow {
                ShiftKey { weight: functionKeyWeight }
                Key { text: "\u314B" }
                Key { text: "\u314C" }
                Key { text: "\u314A" }
                Key { text: "\u314D" }
                Key { text: "\u3160" }
                Key { text: "\u315C" }
                Key { text: "\u3161" }
                BackspaceKey { weight: functionKeyWeight }
            }

            KeyboardRow {
                SymbolModeKey { weight: functionKeyWeight }
                ChangeLanguageKey { weight: defaultKeyWeight }
                Key { key: Qt.Key_Comma; weight: defaultKeyWeight; text: ","; secondaryStyle: true }
                SpaceKey { weight: 300 }
                Key { key: Qt.Key_Period; weight: defaultKeyWeight; text: "."; alternativeKeys: "!.?"; smallText: "!?"; secondaryStyle: true }
                HideKeyboardKey { weight: defaultKeyWeight }
                EnterKey { weight: functionKeyWeight }
            }
        }
    }
}
