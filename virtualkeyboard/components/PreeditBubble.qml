/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.plasma.keyboard.virtualkeyboard

Item {
    id: root

    property var inputEngine: VirtualKeyboard.inputEngine
    readonly property bool hasPreedit: inputEngine && inputEngine.preeditText.length > 0
    readonly property string preeditPrefix: inputEngine ? inputEngine.preeditPrefix : ""
    readonly property string preeditSuffix: {
        if (!inputEngine) {
            return "";
        }
        if (preeditPrefix.length > 0 && inputEngine.preeditText.indexOf(preeditPrefix) === 0) {
            return inputEngine.preeditText.slice(preeditPrefix.length);
        }
        return inputEngine.preeditText;
    }

    visible: hasPreedit && inputEngine.preeditBubbleVisibleHint
    implicitWidth: Math.round(preeditRow.implicitWidth + horizontalPadding * 2)
    implicitHeight: Math.round(preeditRow.implicitHeight + verticalPadding * 2)

    readonly property real horizontalPadding: Math.round(20 * BreezeConstants.scaleHint)
    readonly property real verticalPadding: Math.round(14 * BreezeConstants.scaleHint)

    PopupBackground {
        anchors.fill: parent
        theme: BreezeConstants
    }

    Row {
        id: preeditRow
        anchors.left: parent.left
        anchors.leftMargin: root.horizontalPadding
        anchors.verticalCenter: parent.verticalCenter
        spacing: Math.round(8 * BreezeConstants.scaleHint)

        QQC2.Label {
            visible: root.preeditPrefix.length > 0
            text: root.preeditPrefix
            color: BreezeConstants.popupTextColor
            opacity: 0.7
            font.family: BreezeConstants.fontFamily
            font.weight: Font.Medium
            font.pixelSize: Math.round(34 * BreezeConstants.scaleHint)
        }

        QQC2.Label {
            text: root.preeditSuffix
            color: BreezeConstants.popupTextColor
            opacity: 0.95
            font.family: BreezeConstants.fontFamily
            font.weight: Font.Light
            font.pixelSize: Math.round(34 * BreezeConstants.scaleHint)
        }
    }
}
