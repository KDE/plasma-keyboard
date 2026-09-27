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

    anchors.fill: parent
    anchors.leftMargin: BreezeConstants.keyBackgroundHorizontalMargin + Kirigami.Units.largeSpacing
    anchors.rightMargin: BreezeConstants.keyBackgroundHorizontalMargin + Kirigami.Units.largeSpacing
    spacing: Kirigami.Units.largeSpacing

    Kirigami.Icon {
        source: "search"
        implicitWidth: root.keyItem.iconSize
        implicitHeight: root.keyItem.iconSize
    }

    QQC2.Label {
        Layout.fillWidth: true
        text: root.text.length > 0 ? root.text : i18nc("@info:placeholder", "Search emoji")
        color: BreezeConstants.keyTextColor
        opacity: root.text.length > 0 ? 1 : 0.7
        elide: root.text.length > 0 ? Text.ElideLeft : Text.ElideRight
        font {
            family: BreezeConstants.fontFamily
            weight: Font.Light
            pixelSize: Math.round(40 * root.keyItem.scaleHint)
        }
    }
}
