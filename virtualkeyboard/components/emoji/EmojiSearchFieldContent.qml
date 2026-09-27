/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

// EmojiSearchStrip's search field

RowLayout {
    id: root

    required property Item keyItem

    property string text: ""

    property int cursorPosition: text.length
    property bool showCursor: false

    anchors.fill: parent
    anchors.leftMargin: BreezeConstants.keyBackgroundHorizontalMargin + Kirigami.Units.largeSpacing
    anchors.rightMargin: BreezeConstants.keyBackgroundHorizontalMargin + Kirigami.Units.largeSpacing
    spacing: Kirigami.Units.largeSpacing

    Kirigami.Icon {
        source: "search"
        implicitWidth: root.keyItem.iconSize
        implicitHeight: root.keyItem.iconSize
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true

        QQC2.Label {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            visible: root.text.length === 0
            text: i18nc("@info:placeholder", "Search emoji")
            color: BreezeConstants.keyTextColor
            opacity: 0.7
            elide: Text.ElideRight
            font: queryInput.font
        }

        TextInput {
            id: queryInput

            anchors.fill: parent
            verticalAlignment: TextInput.AlignVCenter
            clip: true
            readOnly: true
            activeFocusOnPress: false
            cursorVisible: root.showCursor
            color: BreezeConstants.keyTextColor
            font {
                family: BreezeConstants.fontFamily
                weight: Font.Light
                pixelSize: Math.round(40 * root.keyItem.scaleHint)
            }

            function sync() {
                text = root.text;
                cursorPosition = root.cursorPosition;
            }
            Component.onCompleted: sync()

            cursorDelegate: Rectangle {
                id: cursor
                property bool blinkOn: true

                width: Math.max(1, Math.round(3 * root.keyItem.scaleHint))
                color: BreezeConstants.keyTextColor
                opacity: blinkOn ? 1 : 0

                Timer {
                    interval: Application.styleHints.cursorFlashTime / 2
                    running: root.showCursor && root.visible && interval > 0
                    repeat: true
                    onTriggered: cursor.blinkOn = !cursor.blinkOn
                    onRunningChanged: cursor.blinkOn = true
                }

                onXChanged: blinkOn = true
            }

            Connections {
                target: root

                function onTextChanged() { queryInput.sync(); }
                function onCursorPositionChanged() { queryInput.sync(); }
            }

            MouseArea { anchors.fill: parent }
        }
    }
}
