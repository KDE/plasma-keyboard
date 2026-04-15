// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2

// Popup that shows up when holding a flick key.

Item {
    id: root

    property Item ownerKey: null
    readonly property bool active: !!ownerKey
    readonly property string centerText: ownerKey ? ownerKey.currentText : ""
    readonly property string flickLeft: ownerKey ? ownerKey.flickLeft : ""
    readonly property string flickTop: ownerKey ? ownerKey.flickTop : ""
    readonly property string flickRight: ownerKey ? ownerKey.flickRight : ""
    readonly property string flickBottom: ownerKey ? ownerKey.flickBottom : ""
    readonly property bool flickKeysVisible: centerText.length > 0
                                            && (flickLeft.length > 0 || flickTop.length > 0 || flickRight.length > 0 || flickBottom.length > 0)
                                            && centerText !== flickLeft
                                            && centerText !== flickTop
                                            && centerText !== flickRight
                                            && centerText !== flickBottom
    readonly property real popupMargin: Math.round(10 * BreezeConstants.scaleHint)
    readonly property real largeTextSize: Math.round((flickKeysVisible ? 66 : 82) * BreezeConstants.scaleHint)
    readonly property real smallTextSize: Math.round((flickKeysVisible ? 50 : 62) * BreezeConstants.scaleHint)
    readonly property real smallTextMargin: Math.round(6 * BreezeConstants.scaleHint)

    visible: active
    z: 5

    function openForKey(keyItem) {
        if (!keyItem || !keyItem.showPreview) {
            return false
        }

        ownerKey = keyItem
        updateFromKey(keyItem)
        return true
    }

    function updateFromKey(keyItem) {
        if (!keyItem || ownerKey !== keyItem) {
            return
        }

        const point = keyItem.mapToItem(root.parent, 0, 0)
        x = point.x
        y = point.y - height - popupMargin
    }

    function close() {
        ownerKey = null
    }

    width: ownerKey ? ownerKey.width : 0
    height: ownerKey ? ownerKey.height : 0

    PopupBackground {
        anchors.fill: parent
        theme: BreezeConstants
    }

    QQC2.Label {
        id: centerLabel
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        text: root.centerText
        color: BreezeConstants.popupTextColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: BreezeConstants.fontFamily
        font.weight: Font.Light
        font.pixelSize: root.flickKeysVisible ? root.smallTextSize : root.largeTextSize
    }

    QQC2.Label {
        anchors.left: parent.left
        anchors.leftMargin: root.smallTextMargin
        anchors.verticalCenter: parent.verticalCenter
        visible: root.flickKeysVisible
        text: root.flickLeft
        color: BreezeConstants.popupTextColor
        opacity: 0.8
        font.family: BreezeConstants.fontFamily
        font.weight: Font.Light
        font.pixelSize: root.smallTextSize
    }

    QQC2.Label {
        anchors.top: parent.top
        anchors.topMargin: root.smallTextMargin
        anchors.horizontalCenter: parent.horizontalCenter
        visible: root.flickKeysVisible
        text: root.flickTop
        color: BreezeConstants.popupTextColor
        opacity: 0.8
        font.family: BreezeConstants.fontFamily
        font.weight: Font.Light
        font.pixelSize: root.smallTextSize
    }

    QQC2.Label {
        anchors.right: parent.right
        anchors.rightMargin: root.smallTextMargin
        anchors.verticalCenter: parent.verticalCenter
        visible: root.flickKeysVisible
        text: root.flickRight
        color: BreezeConstants.popupTextColor
        opacity: 0.8
        font.family: BreezeConstants.fontFamily
        font.weight: Font.Light
        font.pixelSize: root.smallTextSize
    }

    QQC2.Label {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.smallTextMargin
        anchors.horizontalCenter: parent.horizontalCenter
        visible: root.flickKeysVisible
        text: root.flickBottom
        color: BreezeConstants.popupTextColor
        opacity: 0.8
        font.family: BreezeConstants.fontFamily
        font.weight: Font.Light
        font.pixelSize: root.smallTextSize
    }
}
