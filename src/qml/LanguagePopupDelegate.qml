/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import QtQuick.Templates as T

import org.kde.kirigami as Kirigami

T.ItemDelegate {
    id: itemDelegate

    property var style

    horizontalPadding: padding
    verticalPadding: padding
    leftPadding: horizontalPadding
    rightPadding: horizontalPadding
    topPadding: verticalPadding
    bottomPadding: verticalPadding

    highlighted: ListView.isCurrentItem
    hoverEnabled: true

    background: Rectangle {
        color: {
            if (itemDelegate.down) {
                return border.color;
            }
            if (itemDelegate.highlighted || (itemDelegate.hovered && !Kirigami.Settings.tabletMode)) {
                return style.theme.popupHighlightColor
            }
            return 'transparent';
        }
        radius: style.theme.buttonRadius
        border.color: {
            if (itemDelegate.down || itemDelegate.highlighted || (itemDelegate.hovered && !Kirigami.Settings.tabletMode)) {
                return style.theme.popupHighlightBorderColor;
            }
            return 'transparent';
        }
        border.width: 1
    }

    contentItem: Item {
        anchors.fill: parent
        width: itemDelegate.width

        RowLayout {
            id: row
            anchors.fill: parent
            anchors.leftMargin: itemDelegate.leftPadding
            anchors.rightMargin: itemDelegate.rightPadding
            spacing: 12 * style.scaleHint

            Kirigami.Icon {
                id: icon

                visible: itemDelegate.icon.name
                source: itemDelegate.icon.name
                implicitWidth: 44 * style.scaleHint
                implicitHeight: 44 * style.scaleHint
            }

            QQC2.Label {
                text: itemDelegate.text
                font {
                    family: itemDelegate.style.theme.fontFamily
                    weight: Font.Light
                    pixelSize: Math.round(44 * itemDelegate.style.scaleHint)
                }
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
                Layout.maximumWidth: itemDelegate.width - itemDelegate.leftPadding - itemDelegate.rightPadding - (icon.visible ? icon.width + row.spacing : 0)
                elide: Text.ElideRight
            }
        }
    }
}
