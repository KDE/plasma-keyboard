// SPDX-FileCopyrightText: 2021 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KeyboardLayout {
    id: root

    textComposer: ZhuyinTextComposer {}
    sharedLayouts: ["symbols"]

    readonly property real functionKeyWeight: defaultKeyWeight * 1.5

    KeyboardRow {
        Key { text: "\u3105"; alternativeKeys: "\u31051"; smallText: "1" }
        Key { text: "\u3109"; alternativeKeys: "\u31092"; smallText: "2" }
        Key { text: "\u02C7"; alternativeKeys: "\u02C73"; smallText: "3" }
        Key { text: "\u02CB"; alternativeKeys: "\u02CB4"; displayText: "`"; smallText: "4" }
        Key { text: "\u3113"; alternativeKeys: "\u31135"; smallText: "5" }
        Key { text: "\u02CA"; alternativeKeys: "\u02CA6"; displayText: "\u00B4"; smallText: "6" }
        Key { text: "\u02D9"; alternativeKeys: "\u02D97"; smallText: "7" }
        Key { text: "\u311A"; alternativeKeys: "\u311A8"; smallText: "8" }
        Key { text: "\u311E"; alternativeKeys: "\u311E9"; smallText: "9" }
        Key { text: "\u3122"; alternativeKeys: "\u31220"; smallText: "0" }
    }

    KeyboardRow {
        Key { text: "\u3106"; alternativeKeys: "\u3106qQ"; smallText: "q" }
        Key { text: "\u310A"; alternativeKeys: "\u310AwW"; smallText: "w" }
        Key { text: "\u310D"; alternativeKeys: "\u310DeE"; smallText: "e" }
        Key { text: "\u3110"; alternativeKeys: "\u3110rR"; smallText: "r" }
        Key { text: "\u3114"; alternativeKeys: "\u3114tT"; smallText: "t" }
        Key { text: "\u3117"; alternativeKeys: "\u3117yY"; smallText: "y" }
        Key { text: "\u3127"; alternativeKeys: "\u3127uU"; smallText: "u" }
        Key { text: "\u311B"; alternativeKeys: "\u311BiI"; smallText: "i" }
        Key { text: "\u311F"; alternativeKeys: "\u311FoO"; smallText: "o" }
        Key { text: "\u3123"; alternativeKeys: "\u3123pP"; smallText: "p" }
    }

    KeyboardRow {
        Key { text: "\u3107"; alternativeKeys: "\u3107aA"; smallText: "a" }
        Key { text: "\u310B"; alternativeKeys: "\u310BsS"; smallText: "s" }
        Key { text: "\u310E"; alternativeKeys: "\u310EdD"; smallText: "d" }
        Key { text: "\u3111"; alternativeKeys: "\u3111fF"; smallText: "f" }
        Key { text: "\u3115"; alternativeKeys: "\u3115gG"; smallText: "g" }
        Key { text: "\u3118"; alternativeKeys: "\u3118hH"; smallText: "h" }
        Key { text: "\u3128"; alternativeKeys: "\u3128jJ"; smallText: "j" }
        Key { text: "\u311C"; alternativeKeys: "\u311CkK"; smallText: "k" }
        Key { text: "\u3120"; alternativeKeys: "\u3120lL"; smallText: "l" }
        Key { text: "\u3124"; alternativeKeys: "\u3124\u2026"; smallText: "\u2026" }
    }

    KeyboardRow {
        Key { text: "\u3108"; alternativeKeys: "\u3108zZ"; smallText: "z" }
        Key { text: "\u310C"; alternativeKeys: "\u310CxX"; smallText: "x" }
        Key { text: "\u310F"; alternativeKeys: "\u310FcC"; smallText: "c" }
        Key { text: "\u3112"; alternativeKeys: "\u3112vV"; smallText: "v" }
        Key { text: "\u3116"; alternativeKeys: "\u3116bB"; smallText: "b" }
        Key { text: "\u3119"; alternativeKeys: "\u3119nN"; smallText: "n" }
        Key { text: "\u3129"; alternativeKeys: "\u3129mM"; smallText: "m" }
        Key { text: "\u311D"; alternativeKeys: "\u3001\u311D\uFF0C"; smallText: "\uFF0C" }
        Key { text: "\u3121"; alternativeKeys: "\u3002\u3121\uFF0E"; smallText: "\uFF0E" }
        Key { text: "\u3125"; alternativeKeys: "\uFF1B\uFF1A\u3125\u3126"; smallText: "\u3126" }
    }

    KeyboardRow {
        SymbolModeKey {
            weight: functionKeyWeight
        }
        ChangeLanguageKey {
            weight: defaultKeyWeight
        }
        Key {
            key: Qt.Key_Comma
            weight: defaultKeyWeight
            text: "\uFF0C"
            secondaryStyle: true
        }
        SpaceKey { weight: 200 }
        Key {
            key: Qt.Key_Period
            weight: defaultKeyWeight
            text: "\u3002"
            alternativeKeys: "\uFF1B\u3001\u3002\uFF1A\uFF0E？！"
            smallText: "!?"
            secondaryStyle: true
        }
        BackspaceKey {
            weight: defaultKeyWeight
        }
        HideKeyboardKey {
            weight: defaultKeyWeight
        }
        EnterKey {
            weight: functionKeyWeight
        }
    }
}
