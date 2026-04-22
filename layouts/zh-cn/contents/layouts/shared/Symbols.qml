// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayoutLoader {
    id: root

    packageId: parent && parent.packageId !== undefined ? parent.packageId : ""
    property int page
    readonly property int numPages: 3
    sourceComponent: {
        switch (page) {
        case 2: return page3
        case 1: return page2
        default: return page1
        }
    }

    Component {
        id: page1
        KeyboardLayout {
            sharedLayouts: ["main"]
            readonly property real functionKeyWeight: defaultKeyWeight * 1.5
            KeyboardRow {
                Key { key: Qt.Key_1; text: "1" }
                Key { key: Qt.Key_2; text: "2" }
                Key { key: Qt.Key_3; text: "3" }
                Key { key: Qt.Key_4; text: "4" }
                Key { key: Qt.Key_5; text: "5" }
                Key { key: Qt.Key_6; text: "6" }
                Key { key: Qt.Key_7; text: "7" }
                Key { key: Qt.Key_8; text: "8" }
                Key { key: Qt.Key_9; text: "9" }
                Key { key: Qt.Key_0; text: "0" }
            }
            KeyboardRow {
                Key { text: "@" }
                Key { text: "#" }
                Key { text: "%" }
                Key { text: "&" }
                Key { text: "*" }
                Key { text: "_" }
                Key { text: "-" }
                Key { text: "+" }
                Key { text: "(" }
                Key { text: ")" }
            }
            KeyboardRow {
                Key { displayText: (page + 1) + "/" + numPages; functionKey: true; noKeyEvent: true; onClicked: page = (page + 1) % numPages; secondaryStyle: true }
                Key { text: "“" }
                Key { text: "”" }
                Key { text: "、" }
                Key { text: "：" }
                Key { text: "；" }
                Key { text: "！" }
                Key { text: "？" }
                Key { text: "～" }
                BackspaceKey {}
            }
            KeyboardRow {
                SymbolModeKey { weight: functionKeyWeight; displayText: "ABC" }
                ChangeLanguageKey { weight: defaultKeyWeight }
                Key { key: Qt.Key_Comma; weight: defaultKeyWeight; text: ","; secondaryStyle: true }
                SpaceKey { weight: defaultKeyWeight * 3 }
                Key { key: Qt.Key_Period; weight: defaultKeyWeight; text: "—"; secondaryStyle: true }
                HideKeyboardKey { weight: defaultKeyWeight }
                EnterKey { weight: functionKeyWeight }
            }
        }
    }

    Component {
        id: page2
        KeyboardLayout {
            sharedLayouts: ["main"]
            readonly property real functionKeyWeight: defaultKeyWeight * 1.5
            KeyboardRow {
                Key { text: "½"; alternativeKeys: "½¼¾" }
                Key { text: "'" }
                Key { text: "/" }
                Key { text: "\\" }
                Key { text: "|" }
                Key { text: "[" }
                Key { text: "]" }
                Key { text: "{" }
                Key { text: "}" }
                Key { text: "·" }
            }
            KeyboardRow {
                Key { text: "<" }
                Key { text: ">" }
                Key { text: "," }
                Key { text: "." }
                Key { text: ":" }
                Key { text: ";" }
                Key { text: "!" }
                Key { text: "?" }
                Key { text: "=" }
                Key { text: "~" }
            }
            KeyboardRow {
                Key { displayText: (page + 1) + "/" + numPages; functionKey: true; noKeyEvent: true; onClicked: page = (page + 1) % numPages; secondaryStyle: true }
                Key { text: "\"" }
                Key { text: "§" }
                Key { text: "^" }
                Key { text: "$" }
                Key { text: "￥" }
                Key { text: "€" }
                Key { text: "£" }
                Key { text: "¢" }
                BackspaceKey {}
            }
            KeyboardRow {
                SymbolModeKey { weight: functionKeyWeight; displayText: "ABC" }
                ChangeLanguageKey { weight: defaultKeyWeight }
                Key { key: Qt.Key_Comma; weight: defaultKeyWeight; text: ","; secondaryStyle: true }
                SpaceKey { weight: defaultKeyWeight * 3 }
                Key { key: Qt.Key_Period; weight: defaultKeyWeight; text: "。"; secondaryStyle: true }
                HideKeyboardKey { weight: defaultKeyWeight }
                EnterKey { weight: functionKeyWeight }
            }
        }
    }

    Component {
        id: page3
        KeyboardLayout {
            sharedLayouts: ["main"]
            readonly property real functionKeyWeight: defaultKeyWeight * 1.5
            KeyboardRow {
                Key { text: "＼" }
                Key { text: "／" }
                Key { text: "（" }
                Key { text: "）" }
                Key { text: "〔" }
                Key { text: "〕" }
                Key { text: "〈" }
                Key { text: "〉" }
                Key { text: "《" }
                Key { text: "》" }
            }
            KeyboardRow {
                Key { text: "→" }
                Key { text: "←" }
                Key { text: "↑" }
                Key { text: "↓" }
                Key { text: "■" }
                Key { text: "□" }
                Key { text: "●" }
                Key { text: "○" }
                Key { text: "【" }
                Key { text: "】" }
            }
            KeyboardRow {
                Key { displayText: (page + 1) + "/" + numPages; functionKey: true; noKeyEvent: true; onClicked: page = (page + 1) % numPages; secondaryStyle: true }
                Key { text: "『" }
                Key { text: "』" }
                Key { text: "「" }
                Key { text: "」" }
                Key { text: "★" }
                Key { text: "☆" }
                Key { text: "◆" }
                Key { text: "◇" }
                BackspaceKey {}
            }
            KeyboardRow {
                SymbolModeKey { weight: functionKeyWeight; displayText: "ABC" }
                ChangeLanguageKey { weight: defaultKeyWeight }
                Key { key: Qt.Key_Comma; weight: defaultKeyWeight; text: ","; secondaryStyle: true }
                SpaceKey { weight: defaultKeyWeight * 3 }
                Key { key: Qt.Key_Period; weight: defaultKeyWeight; text: "…"; secondaryStyle: true }
                HideKeyboardKey { weight: defaultKeyWeight }
                EnterKey { weight: functionKeyWeight }
            }
        }
    }
}
