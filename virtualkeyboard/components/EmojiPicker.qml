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

pragma ComponentBehavior: Bound

Item {
    id: root

    required property var emojiController
    required property var inputEngine

    readonly property font emojiFont: Qt.font({
        family: "emoji",
        pixelSize: Math.max(1, Math.round(58 * BreezeConstants.scaleHint))
    })
    readonly property real toolbarHeight: Math.max(Math.round(88 * BreezeConstants.scaleHint), searchButton.implicitHeight, abcButton.implicitHeight, backspaceButton.implicitHeight)
    readonly property real categoryBarHeight: Math.max(Math.round(80 * BreezeConstants.scaleHint), Kirigami.Units.gridUnit * 2)
    readonly property real emojiCellSize: emojiMetrics.height + Kirigami.Units.mediumSpacing * 2

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

    Component.onCompleted: {
        emojiGrid.model.currentCategory = KEmoji.Categories.Recent;
        if (emojiGrid.model.rowCount() === 0) {
            emojiGrid.model.currentCategory = KEmoji.Categories.All;
        }
    }

    ColumnLayout {
        id: pickerLayout

        anchors.fill: parent
        spacing: 0

        RowLayout {
            id: pickerToolbar

            Layout.fillWidth: true
            Layout.minimumHeight: root.toolbarHeight
            Layout.preferredHeight: root.toolbarHeight
            Layout.maximumHeight: root.toolbarHeight
            spacing: Math.round(8 * BreezeConstants.scaleHint)

            QQC2.ToolButton {
                id: abcButton

                Layout.fillHeight: true
                text: i18nc("@action:button", "Back")
                icon.name: root.LayoutMirroring.enabled ? "go-next-symbolic" : "go-previous-symbolic"
                display: QQC2.AbstractButton.IconOnly
                onClicked: root.emojiController.close()
            }

            QQC2.ToolButton {
                id: searchButton

                Layout.fillWidth: true
                Layout.fillHeight: true
                text: i18nc("@action:button", "Search emoji")
                icon.name: "search"
                onClicked: root.emojiController.startSearch()

                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Kirigami.Icon {
                        source: "search"
                        implicitWidth: Math.round(40 * BreezeConstants.scaleHint)
                        implicitHeight: implicitWidth
                    }
                    QQC2.Label {
                        Layout.fillWidth: true
                        text: i18nc("@action:button", "Search emoji")
                        color: BreezeConstants.keyTextColor
                        elide: Text.ElideRight
                        font.family: BreezeConstants.fontFamily
                    }
                }
            }

            QQC2.ToolButton {
                id: backspaceButton

                Layout.fillHeight: true
                icon.name: "edit-clear-symbolic"
                text: i18nc("@action:button", "Backspace")
                display: QQC2.AbstractButton.IconOnly
                onClicked: root.inputEngine.sendTextComposerKey(Qt.Key_Backspace, "")

                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.text: text
            }
        }

        KEmoji.EmojiGrid {
            id: emojiGrid

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 0
            clip: true
            font: root.emojiFont
            showUnsupportedEmojis: false

            delegate: KEmoji.EmojiDelegate {
                id: emojiDelegate

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

        ListView {
            id: categoryView

            Layout.fillWidth: true
            Layout.minimumHeight: root.categoryBarHeight
            Layout.preferredHeight: root.categoryBarHeight
            Layout.maximumHeight: root.categoryBarHeight
            orientation: ListView.Horizontal
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            spacing: Kirigami.Units.smallSpacing
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

            delegate: QQC2.ToolButton {
                id: categoryButton

                required property int modelData

                KEmoji.Category.category: modelData

                width: Math.max(categoryView.height, implicitWidth)
                height: categoryView.height
                icon.name: KEmoji.Category.iconName
                text: KEmoji.Category.name
                display: QQC2.AbstractButton.IconOnly
                checked: emojiGrid.model.currentCategory === modelData
                onClicked: emojiGrid.model.currentCategory = modelData

                Accessible.name: text
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.text: text
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

            delegate: KEmoji.EmojiDelegate {
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
