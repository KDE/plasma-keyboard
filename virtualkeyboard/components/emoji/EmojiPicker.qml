/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kemoji as KEmoji
import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

pragma ComponentBehavior: Bound

// The main emoji picker view.

Item {
    id: root

    required property var emojiController
    required property var inputEngine

    readonly property font emojiFont: Qt.font({
        family: "emoji",
        pixelSize: Math.max(24, Math.round(58 * BreezeConstants.scaleHint))
    })
    readonly property real toolbarHeight: Math.max(Math.round(110 * BreezeConstants.scaleHint), Kirigami.Units.gridUnit * 2)
    readonly property real categoryBarHeight: Math.max(Math.round(100 * BreezeConstants.scaleHint), Kirigami.Units.gridUnit * 2)
    readonly property real emojiCellSize: emojiMetrics.height + Kirigami.Units.smallSpacing * 2

    FontMetrics {
        id: emojiMetrics
        font: root.emojiFont
    }

    function commitEmoji(emoji) {
        if (emoji.unicode.length === 0) {
            return;
        }
        Feedback.play(Feedback.SelectionCommit);
        emojiController.commitEmoji(emoji.unicode);
    }

    // Show the first category scrolled to the top
    function resetView() {
        variantPopup.close();
        emojiGrid.model.currentCategory = KEmoji.Categories.Recent;
        if (emojiGrid.model.rowCount() === 0) {
            emojiGrid.model.currentCategory = KEmoji.Categories.All;
        }
        emojiGrid.currentIndex = -1;
        emojiGrid.positionViewAtBeginning();
    }

    Component.onCompleted: resetView()

    Connections {
        target: root.emojiController

        function onActiveChanged() {
            if (root.emojiController.active) {
                root.resetView();
            }
        }
    }

    ColumnLayout {
        id: pickerLayout

        anchors.fill: parent
        spacing: 0

        KeyboardRow {
            id: pickerToolbar

            Layout.minimumHeight: root.toolbarHeight
            Layout.preferredHeight: root.toolbarHeight
            Layout.maximumHeight: root.toolbarHeight
            Layout.fillHeight: false

            ActionKey {
                id: backKey

                weight: 150
                iconName: root.LayoutMirroring.enabled ? "go-next-symbolic" : "go-previous-symbolic"
                functionKey: true
                secondaryStyle: true
                onClicked: root.emojiController.close()

                Accessible.role: Accessible.Button
                Accessible.name: i18nc("@action:button", "Back")
            }

            ActionKey {
                id: searchKey

                weight: 700
                functionKey: true
                onClicked: root.emojiController.startSearch()

                Accessible.role: Accessible.Button
                Accessible.name: i18nc("@action:button", "Search emoji")

                EmojiSearchFieldContent {
                    keyItem: searchKey
                }
            }

            BackspaceKey {
                id: backspaceKey

                weight: 150

                Accessible.role: Accessible.Button
                Accessible.name: i18nc("@action:button", "Backspace")
            }
        }

        KEmoji.EmojiGrid {
            id: emojiGrid

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 0

            readonly property int columnCount: Math.max(1, Math.floor(width / root.emojiCellSize))

            clip: true
            font: root.emojiFont
            showUnsupportedEmojis: false
            cellWidth: Math.floor(width / columnCount)
            cellHeight: root.emojiCellSize
            leftMargin: Math.floor((width - cellWidth * columnCount) / 2)

            delegate: EmojiPickerDelegate {
                id: emojiDelegate

                width: emojiGrid.cellWidth
                height: emojiGrid.cellHeight
                font: root.emojiFont
                highlighted: GridView.isCurrentItem || variantPopup.owner === emojiDelegate
                onClicked: root.commitEmoji(emojiDelegate.emoji)
                onPressAndHold: {
                    if (emojiDelegate.variantEmojis.size > 0) {
                        variantPopup.openFor(emojiDelegate, emojiDelegate.variantEmojis)
                    }
                }
            }
        }

        KeyboardRow {
            id: categoryBar

            Layout.minimumHeight: root.categoryBarHeight
            Layout.preferredHeight: root.categoryBarHeight
            Layout.maximumHeight: root.categoryBarHeight
            Layout.fillHeight: false

            Repeater {
                model: [
                    KEmoji.Categories.Recent,
                    KEmoji.Categories.Smileys,
                    KEmoji.Categories.People,
                    KEmoji.Categories.Animals,
                    KEmoji.Categories.Food,
                    KEmoji.Categories.Travel,
                    KEmoji.Categories.Activities,
                    KEmoji.Categories.Objects,
                    KEmoji.Categories.Symbols,
                    KEmoji.Categories.Flags
                ]

                delegate: ActionKey {
                    id: categoryKey

                    required property int modelData

                    KEmoji.Category.category: modelData

                    iconName: KEmoji.Category.iconName
                    functionKey: true
                    secondaryStyle: !highlighted
                    highlighted: emojiGrid.model.currentCategory === modelData
                    onClicked: emojiGrid.model.currentCategory = modelData

                    Accessible.role: Accessible.Button
                    Accessible.name: KEmoji.Category.name
                }
            }
        }
    }

    QQC2.Popup {
        id: variantPopup

        property Item owner: null
        property KEmoji.group emojis
        readonly property int columnCount: Math.min(emojis.size, 5)
        readonly property int rowCount: Math.ceil(emojis.size / 5)

        parent: root
        padding: Kirigami.Units.smallSpacing
        modal: true
        dim: false
        focus: false
        popupType: QQC2.Popup.Item
        closePolicy: QQC2.Popup.CloseOnEscape | QQC2.Popup.CloseOnPressOutside
        width: root.emojiCellSize * columnCount + leftPadding + rightPadding
        height: root.emojiCellSize * rowCount + topPadding + bottomPadding

        background: PopupBackground {
            theme: BreezeConstants
        }

        function openFor(delegate, variantEmojis) {
            owner = delegate
            emojis = variantEmojis

            const point = delegate.mapToItem(root, 0, 0)
            x = Math.max(0, Math.min(root.width - width, point.x + (delegate.width - width) / 2))
            const aboveY = point.y - height
            y = aboveY >= 0 ? aboveY : Math.min(root.height - height, point.y + delegate.height)
            open()
        }

        onClosed: owner = null

        contentItem: GridView {
            id: variantView

            cellWidth: root.emojiCellSize
            cellHeight: root.emojiCellSize
            interactive: false
            model: KEmoji.Model {
                emojis: variantPopup.emojis
            }

            delegate: EmojiPickerDelegate {
                id: variantDelegate

                font: root.emojiFont
                showVariantEmojis: false
                onClicked: {
                    variantPopup.close()
                    root.commitEmoji(variantDelegate.emoji)
                }
            }
        }
    }
}
