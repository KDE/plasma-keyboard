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

Rectangle {
    id: root

    required property var emojiController
    readonly property string query: emojiController ? emojiController.query : ""
    readonly property font emojiFont: Qt.font({
        family: "emoji",
        pixelSize: Math.max(1, Math.round(48 * BreezeConstants.scaleHint))
    })
    readonly property real searchFieldHeight: Math.max(Math.round(76 * BreezeConstants.scaleHint), backButton.implicitHeight)
    readonly property real resultRowHeight: Math.max(Math.round(100 * BreezeConstants.scaleHint), emojiMetrics.height + Kirigami.Units.mediumSpacing * 2)

    implicitHeight: searchFieldHeight + resultRowHeight
    color: BreezeConstants.selectionListBackgroundColor

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

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.minimumHeight: root.searchFieldHeight
            Layout.preferredHeight: root.searchFieldHeight
            Layout.maximumHeight: root.searchFieldHeight
            spacing: Math.round(8 * BreezeConstants.scaleHint)

            QQC2.ToolButton {
                id: backButton

                Layout.fillHeight: true
                icon.name: root.LayoutMirroring.enabled ? "go-next-symbolic" : "go-previous-symbolic"
                text: i18nc("@action:button", "Back to emoji picker")
                display: QQC2.AbstractButton.IconOnly
                onClicked: root.emojiController.stopSearch()

                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.text: text
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: BreezeConstants.buttonRadius
                color: BreezeConstants.normalKeyBackgroundColor

                Kirigami.Icon {
                    id: searchIcon
                    anchors.left: parent.left
                    anchors.leftMargin: Kirigami.Units.smallSpacing
                    anchors.verticalCenter: parent.verticalCenter
                    width: Math.round(36 * BreezeConstants.scaleHint)
                    height: width
                    source: "search"
                }

                QQC2.Label {
                    anchors.left: searchIcon.right
                    anchors.right: parent.right
                    anchors.leftMargin: Kirigami.Units.smallSpacing
                    anchors.rightMargin: Kirigami.Units.smallSpacing
                    anchors.verticalCenter: parent.verticalCenter
                    text: root.query.length > 0 ? root.query : i18nc("@info:placeholder", "Search emoji")
                    color: BreezeConstants.keyTextColor
                    opacity: root.query.length > 0 ? 1 : 0.65
                    elide: Text.ElideLeft
                }
            }

            QQC2.ToolButton {
                Layout.fillHeight: true
                visible: root.query.length > 0
                icon.name: "edit-clear-symbolic"
                text: i18nc("@action:button", "Clear emoji search")
                display: QQC2.AbstractButton.IconOnly
                onClicked: root.emojiController.clearQuery()

                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.text: text
            }
        }

        ListView {
            id: resultView

            Layout.fillWidth: true
            Layout.minimumHeight: root.resultRowHeight
            Layout.preferredHeight: root.resultRowHeight
            Layout.maximumHeight: root.resultRowHeight
            orientation: ListView.Horizontal
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            spacing: 0
            model: KEmoji.SortFilterModel {
                searchText: root.query
                currentCategory: root.query.length > 0 ? KEmoji.Categories.All : KEmoji.Categories.Recent
                currentFont: root.emojiFont
                showUnsupportedEmojis: false
                sourceModel: KEmoji.Model {
                    emojis: KEmoji.Dict.emojis
                }
            }

            delegate: KEmoji.EmojiDelegate {
                id: resultDelegate

                height: resultView.height
                font: root.emojiFont
                showVariantEmojis: false
                onClicked: root.commitEmoji(resultDelegate.emoji)
            }

            QQC2.Label {
                anchors.centerIn: parent
                visible: resultView.count === 0
                text: root.query.length > 0 ? i18nc("@info", "No emoji found") : i18nc("@info", "Type to search for emoji")
                color: BreezeConstants.selectionListTextColor
                opacity: 0.7
                font.family: BreezeConstants.fontFamily
            }
        }
    }
}
