// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.VirtualKeyboard

import org.kde.kirigami as Kirigami

// Key component for our own keyboard simulator keys (ex. Meta, Alt, Ctrl, Esc)
BaseKey {
    id: keyPanel

    property var theme: keyboard.style.theme

    // Whether this key is "pressed" as a modifier (ex. Meta, Alt, Ctrl)
    property bool checked: false

    // The icon to use (as opposed to text)
    property string iconSource

    keyType: QtVirtualKeyboard.KeyType.Key
    functionKey: true

    keyPanelDelegate: BreezeKeyPanel {
        theme: keyPanel.theme

        Item {
            id: keyContent

            QQC2.Label {
                id: keyText
                text: control.displayText
                color: theme.keyTextColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: control.displayText.length > 1 ? Text.AlignVCenter : Text.AlignBottom
                anchors.centerIn: parent
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 40 * keyboard.style.scaleHint
                    capitalization: Font.MixedCase
                }
            }

            Kirigami.Icon {
                id: icon
                source: keyPanel.iconSource
                anchors.centerIn: parent
                implicitHeight: 88 * theme.keyIconScale
                implicitWidth: implicitHeight
            }
        }

        states: [
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: keyContent
                    opacity: 0.75
                }
                PropertyChanges {
                    target: keyText
                    opacity: 0.05
                }
            }
        ]
    }
}
