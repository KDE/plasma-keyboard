// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard

// Popup that shows when you hold a key, and then can swipe left/right to select an alt key.

Item {
    id: root

    property Item keyboardPanel
    property var ownerKey: null
    property var items: []
    property int currentIndex: -1
    readonly property bool active: ownerKey !== null && items.length > 0
    readonly property real itemWidth: Math.round(92 * BreezeConstants.scaleHint)
    readonly property real itemHeight: Math.round(116 * BreezeConstants.scaleHint)
    readonly property real popupMargin: Math.round(16 * BreezeConstants.scaleHint)
    readonly property real bottomMargin: Math.round(10 * BreezeConstants.scaleHint)

    anchors.fill: parent
    visible: active
    z: 3

    function openForKey(keyItem) {
        if (!keyboardPanel || !keyItem) {
            return false;
        }

        const alternativeKeys = keyItem.effectiveAlternativeKeys || [];
        const displayAlternativeKeys = keyItem.displayAlternativeKeys || alternativeKeys;
        if (alternativeKeys.length === 0 || displayAlternativeKeys.length !== alternativeKeys.length) {
            return false;
        }

        const entries = [];
        for (let i = 0; i < alternativeKeys.length; ++i) {
            entries.push({
                text: keyItem.uppercased ? String(displayAlternativeKeys[i]).toUpperCase() : String(displayAlternativeKeys[i]),
                data: keyItem.uppercased ? String(alternativeKeys[i]).toUpperCase() : String(alternativeKeys[i])
            });
        }

        ownerKey = keyItem;
        items = entries;
        currentIndex = Math.max(0, Math.min(entries.length - 1, keyItem.effectiveAlternativeKeysHighlightIndex));

        const point = keyItem.mapToItem(root, keyItem.width / 2, 0);
        const popupWidth = listRow.width + popupMargin * 2;
        const maxX = Math.max(0, root.width - popupWidth);
        const currentItemOffset = popupMargin + (currentIndex + 0.5) * itemWidth;
        const desiredX = point.x - currentItemOffset;
        popupContainer.x = Math.max(0, Math.min(maxX, desiredX));
        popupContainer.y = point.y - popupContainer.height - bottomMargin;
        return true;
    }

    function close() {
        ownerKey = null;
        items = [];
        currentIndex = -1;
    }

    function moveFromKey(keyItem, localX) {
        if (!active || ownerKey !== keyItem) {
            return;
        }

        const point = keyItem.mapToItem(listRow, localX, 0);
        updateCurrentIndex(point.x);
    }

    function moveFromPanelPosition(panelX) {
        if (!active) {
            return;
        }

        const point = root.mapToItem(listRow, panelX, 0);
        updateCurrentIndex(point.x);
    }

    function updateCurrentIndex(contentX) {
        const clampedX = Math.max(1, Math.min(listRow.width - 1, contentX));
        const nextIndex = Math.max(0, Math.min(items.length - 1, Math.floor(clampedX / root.itemWidth)));
        if (nextIndex === currentIndex) {
            return;
        }

        currentIndex = nextIndex;
        Feedback.play(Feedback.SelectionChange);
    }

    function commitCurrent() {
        if (!active || currentIndex < 0 || currentIndex >= items.length || !ownerKey) {
            close();
            return;
        }

        const entry = items[currentIndex];
        Feedback.play(Feedback.SelectionCommit);
        ownerKey.commitAlternativeText(entry.data);
        close();
    }

    TapHandler {
        acceptedButtons: Qt.LeftButton
        onTapped: (eventPoint) => {
            if (!popupContainer.contains(mapToItem(popupContainer, eventPoint.position.x, eventPoint.position.y))) {
                root.close();
            }
        }
    }

    Item {
        id: popupContainer
        visible: root.active
        width: listRow.width + root.popupMargin * 2
        height: root.itemHeight + root.popupMargin * 2

        PopupBackground {
            anchors.fill: parent
            theme: BreezeConstants
        }

        Row {
            id: listRow
            anchors.centerIn: parent
            spacing: 0

            Repeater {
                model: root.items

                delegate: Item {
                    required property int index
                    required property var modelData

                    width: root.itemWidth
                    height: root.itemHeight

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: Math.round(4 * BreezeConstants.scaleHint)
                        radius: BreezeConstants.buttonRadius
                        color: root.currentIndex === index ? BreezeConstants.popupHighlightColor : "transparent"
                        border.width: root.currentIndex === index ? 1 : 0
                        border.color: BreezeConstants.popupHighlightBorderColor
                    }

                    QQC2.Label {
                        anchors.centerIn: parent
                        text: modelData.text
                        color: BreezeConstants.popupTextColor
                        font.family: BreezeConstants.fontFamily
                        font.weight: root.currentIndex === index ? Font.Medium : Font.Light
                        font.pixelSize: Math.round(52 * BreezeConstants.scaleHint)
                    }
                }
            }
        }
    }
}
