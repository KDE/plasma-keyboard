// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2

// Character popup shown above a pressed text key.

Item {
    id: root

    property var ownerKey: null

    readonly property bool active: ownerKey !== null
    readonly property string popupText: {
        if (!ownerKey) {
            return "";
        }
        return ownerKey.uppercased ? ownerKey.displayText.toUpperCase() : ownerKey.displayText;
    }
    readonly property real popupMargin: Math.round(10 * BreezeConstants.scaleHint)

    visible: active
    z: 6

    width: ownerKey ? ownerKey.width : 0
    height: ownerKey ? ownerKey.height : 0

    function openForKey(keyItem) {
        if (!enabled || !keyItem || !keyItem.showPreview || keyItem.displayText.length === 0) {
            return false;
        }

        ownerKey = keyItem;
        updateFromKey(keyItem);
        return true;
    }

    function updateFromKey(keyItem) {
        if (!keyItem || ownerKey !== keyItem) {
            return;
        }

        const point = keyItem.mapToItem(root.parent, 0, 0);
        x = point.x;
        y = point.y - height - popupMargin;
    }

    function closeForKey(keyItem) {
        if (ownerKey === keyItem) {
            ownerKey = null;
        }
    }

    function close() {
        ownerKey = null;
    }

    onEnabledChanged: {
        if (!enabled) {
            close();
        }
    }

    PopupBackground {
        anchors.fill: parent
        theme: BreezeConstants
    }

    QQC2.Label {
        anchors.fill: parent
        text: root.popupText
        color: BreezeConstants.popupTextColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: BreezeConstants.fontFamily
        font.weight: Font.Light
        font.pixelSize: root.ownerKey ? Math.round(root.ownerKey.textPixelSize * 1.5) : 0
    }
}
