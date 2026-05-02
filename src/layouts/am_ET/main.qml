/*
    SPDX-FileCopyrightText: 2026 Abenezer Wesenseged <wseged@proton.me>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Components
import QtQuick.Layouts

KeyboardLayout {
    id: keyboardLayout
    keyWeight: 160

    function createInputMethod() {
        return Qt.createComponent("AmharicInputMethod.qml").createObject(parent);
    }

    readonly property real normalKeyWidth: normalKey.width
    readonly property real functionKeyWidth: mapFromItem(normalKey, normalKey.width / 2, 0).x

    KeyboardRow {
        Key {
            text: "\u1200"
        }

        Key {
            id: normalKey
            text: "\u1208"
        }

        Key {
            text: "\u1210"
        }

        Key {
            text: "\u1218"
        }

        Key {
            text: "\u1220"
        }

        Key {
            text: "\u1228"
        }

        Key {
            text: "\u1230"
        }

        Key {
            text: "\u1238"
        }

        Key {
            text: "\u1240"
            alternativeKeys: "\u1248\u124A\u124B\u124C\u124D"
            smallText: "\u1248"
            smallTextVisible: true
        }

        Key {
            text: "\u1260"
        }

        Key {
            text: "\u1268"
        }

        Key {
            text: "\u1270"
        }
    }

    KeyboardRow {
        KeyboardRow {
            Layout.preferredWidth: functionKeyWidth
            Layout.fillWidth: false
            FillerKey {}
            Key {
                text: "\u1278"
                weight: normalKeyWidth
                Layout.fillWidth: false
            }
        }

        Key {
            text: "\u1280"
            alternativeKeys: "\u1288\u128A\u128B\u128C\u128D"
            smallText: "\u1288"
            smallTextVisible: true
        }

        Key {
            text: "\u1290"
        }

        Key {
            text: "\u1298"
        }

        Key {
            text: "\u12A0"
        }

        Key {
            text: "\u12A8"
            alternativeKeys: "\u12B0\u12B2\u12B3\u12B4\u12B5"
            smallText: "\u12B0"
            smallTextVisible: true
        }

        Key {
            text: "\u12B8"
            alternativeKeys: "\u12C0\u12C2\u12C3\u12C4\u12C5"
            smallText: "\u12C0"
            smallTextVisible: true
        }

        Key {
            text: "\u12C8"
        }

        Key {
            text: "\u12D0"
        }

        Key {
            text: "\u12D8"
        }

        KeyboardRow {
            Layout.preferredWidth: functionKeyWidth
            Layout.fillWidth: false
            Key {
                text: "\u12E0"
                weight: normalKeyWidth
                Layout.fillWidth: false
            }
            FillerKey {}
        }
    }

    KeyboardRow {
        Key {
            text: "\u12E8"
        }

        Key {
            text: "\u12F0"
        }

        Key {
            text: "\u1300"
        }

        Key {
            text: "\u1308"
            alternativeKeys: "\u1310\u1312\u1313\u1314\u1315"
            smallText: "\u1310"
            smallTextVisible: true
        }

        Key {
            text: "\u1320"
        }

        Key {
            text: "\u1328"
        }

        Key {
            text: "\u1330"
        }

        Key {
            text: "\u1338"
        }

        Key {
            text: "\u1340"
        }

        Key {
            text: "\u1348"
        }

        Key {
            text: "\u1350"
        }

        BackspaceKey {
            weight: functionKeyWidth
            Layout.fillWidth: false
        }
    }
    KeyboardRow {
        SymbolModeKey {
            weight: functionKeyWidth
            Layout.fillWidth: false
        }
        ChangeLanguageKey {
            weight: normalKeyWidth
            Layout.fillWidth: false
        }
        Key {
            weight: normalKeyWidth
            Layout.fillWidth: false
            text: "\u1363"
            smallText: "\u2699"
            smallTextVisible: keyboard.isFunctionPopupListAvailable()
            highlighted: true
        }
        SpaceKey {}
        Key {
            weight: normalKeyWidth
            Layout.fillWidth: false
            text: "\u1362"
            alternativeKeys: "!.?"
            smallText: "!?"
            smallTextVisible: true
            highlighted: true
        }
        HideKeyboardKey {
            weight: normalKeyWidth
            Layout.fillWidth: false
        }
        EnterKey {
            weight: functionKeyWidth
            Layout.fillWidth: false
        }
    }
}
