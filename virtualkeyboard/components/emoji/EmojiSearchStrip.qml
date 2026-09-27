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

// The component above the keyboard to search for emojis.

Rectangle {
    id: root

    required property var emojiController
    readonly property string query: emojiController ? emojiController.query : ""
    readonly property font emojiFont: Qt.font({
        family: "emoji",
        pixelSize: Math.max(24, Math.round(62 * BreezeConstants.scaleHint))
    })
    readonly property real searchFieldHeight: Math.max(Math.round(110 * BreezeConstants.scaleHint), Kirigami.Units.gridUnit * 2)
    readonly property real resultRowHeight: Math.max(Math.round(100 * BreezeConstants.scaleHint), emojiMetrics.height + Kirigami.Units.smallSpacing * 2)

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

        KeyboardRow {
            Layout.minimumHeight: root.searchFieldHeight
            Layout.preferredHeight: root.searchFieldHeight
            Layout.maximumHeight: root.searchFieldHeight
            Layout.fillHeight: false

            ActionKey {
                id: backKey

                weight: 150
                iconName: root.LayoutMirroring.enabled ? "go-next-symbolic" : "go-previous-symbolic"
                functionKey: true
                secondaryStyle: true
                onClicked: root.emojiController.stopSearch()

                Accessible.role: Accessible.Button
                Accessible.name: i18nc("@action:button", "Back to emoji picker")
            }

            AbstractKey {
                id: searchField

                weight: 850
                functionKey: true

                Accessible.role: Accessible.StaticText
                Accessible.name: root.query.length > 0 ? root.query : i18nc("@info:placeholder", "Search emoji")

                EmojiSearchFieldContent {
                    keyItem: searchField
                    text: root.query
                }
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

            delegate: EmojiPickerDelegate {
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
