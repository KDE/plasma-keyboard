/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Templates as T

import org.kde.kirigami as Kirigami
import org.kde.plasma.keyboard.virtualkeyboard

QQC2.Popup {
    id: root

    property Item keyboardPanel
    property var keyboardController: VirtualKeyboard.keyboardController
    readonly property bool popupVisible: visible
    readonly property int itemCount: languageListView.count
    property int currentIndex: languageListView.currentIndex
    readonly property string currentLayoutId: (currentIndex >= 0 && keyboardController && currentIndex < keyboardController.activeLayoutIds.length)
        ? keyboardController.activeLayoutIds[currentIndex] : ""

    signal showSettings()

    parent: keyboardPanel
    width: Kirigami.Units.gridUnit * 10
    height: Math.min(languageListView.contentHeight, Kirigami.Units.gridUnit * 48, keyboardPanel.height - padding * 2) + padding * 2
    padding: Kirigami.Units.smallSpacing

    modal: true
    dim: false

    background: PopupBackground {
        theme: BreezeConstants
    }

    function showForItem(parentItem) {
        if (!keyboardPanel || !parentItem) {
            return;
        }

        if (!keyboardController) {
            return;
        }

        currentIndex = keyboardController.currentLayoutIndex;
        languageListView.positionViewAtIndex(currentIndex, ListView.Center);

        const point = parentItem.mapToItem(keyboardPanel, 0, 0);
        const maxX = Math.max(0, keyboardPanel.width - width);
        const desiredX = point.x + (parentItem.width - width) / 2;

        x = Math.max(0, Math.min(maxX, desiredX));
        y = Math.max(0, point.y - height);
        open();
    }

    function moveSelection(delta) {
        if (itemCount <= 0) {
            return false;
        }

        const nextIndex = Math.max(0, Math.min(itemCount - 1, currentIndex + delta));
        if (nextIndex === currentIndex) {
            return false;
        }

        currentIndex = nextIndex;
        languageListView.positionViewAtIndex(currentIndex, ListView.Contain);
        return true;
    }

    function activateCurrent() {
        if (!keyboardController || currentIndex < 0 || currentIndex >= keyboardController.activeLayoutIds.length) {
            return false;
        }

        keyboardController.setCurrentLayout(keyboardController.activeLayoutIds[currentIndex]);
        close();
        return true;
    }

    contentItem: ListView {
        id: languageListView
        footerPositioning: ListView.OverlayFooter
        clip: true
        spacing: 0

        readonly property real rowPadding: Kirigami.Units.smallSpacing

        QQC2.ScrollBar.vertical: QQC2.ScrollBar {}

        model: root.keyboardController ? root.keyboardController.activeLayoutIds : []

        delegate: QQC2.ItemDelegate {
            required property string modelData
            property bool navigationActive: root.currentLayoutId === modelData

            width: languageListView.width
            highlighted: root.keyboardController && (root.keyboardController.layoutId === modelData || navigationActive)
            leftPadding: languageListView.rowPadding
            rightPadding: languageListView.rowPadding
            topPadding: languageListView.rowPadding
            bottomPadding: languageListView.rowPadding
            topInset: 0; bottomInset: 0; rightInset: 0; leftInset: 0

            background: Rectangle {
                color: {
                    if (parent.down) {
                        return BreezeConstants.popupHighlightBorderColor
                    }
                    if (parent.highlighted || (parent.hovered && !Kirigami.Settings.tabletMode)) {
                        return BreezeConstants.popupHighlightColor
                    }
                    return "transparent"
                }
                radius: BreezeConstants.buttonRadius
                border.width: 1
                border.color: {
                    if (parent.down || parent.highlighted || (parent.hovered && !Kirigami.Settings.tabletMode)) {
                        return BreezeConstants.popupHighlightBorderColor
                    }
                    return "transparent"
                }
            }

            contentItem: ColumnLayout {
                spacing: 0

                QQC2.Label {
                    id: layoutPackageLabel
                    Layout.fillWidth: true
                    text: root.keyboardController ? root.keyboardController.layoutDisplayName(modelData) : ""
                    color: BreezeConstants.popupTextColor
                    elide: Text.ElideRight
                    font.family: BreezeConstants.fontFamily
                    font.weight: Font.Light
                }

                QQC2.Label {
                    id: layoutLabel
                    Layout.fillWidth: true
                    text: root.keyboardController ? root.keyboardController.layoutName(modelData) : ""
                    color: BreezeConstants.popupTextColor
                    opacity: 0.75
                    elide: Text.ElideRight
                    font.family: BreezeConstants.fontFamily
                    font.weight: Font.Light
                    font.pixelSize: layoutPackageLabel.font.pixelSize * 0.8
                }
            }

            onClicked: {
                if (!root.keyboardController) {
                    return;
                }

                root.currentIndex = root.keyboardController.activeLayoutIds.indexOf(modelData);
                root.keyboardController.setCurrentLayout(modelData);
                root.close();
            }
        }

        footer: Rectangle {
            z: 999
            width: languageListView.width
            height: footerColumn.implicitHeight
            color: BreezeConstants.popupBackgroundColor

            ColumnLayout {
                id: footerColumn
                anchors.fill: parent
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Separator {
                    Layout.fillWidth: true
                }

                QQC2.ItemDelegate {
                    id: settingsButton

                    Layout.fillWidth: true
                    leftPadding: languageListView.rowPadding
                    rightPadding: languageListView.rowPadding
                    topPadding: languageListView.rowPadding
                    bottomPadding: languageListView.rowPadding
                    topInset: 0; bottomInset: 0; rightInset: 0; leftInset: 0

                    background: Rectangle {
                        color: {
                            if (settingsButton.down) {
                                return BreezeConstants.popupHighlightBorderColor
                            }
                            if (settingsButton.hovered && !Kirigami.Settings.tabletMode) {
                                return BreezeConstants.popupHighlightColor
                            }
                            return "transparent"
                        }
                        radius: BreezeConstants.buttonRadius
                        border.width: 1
                        border.color: {
                            if (settingsButton.down || (settingsButton.hovered && !Kirigami.Settings.tabletMode)) {
                                return BreezeConstants.popupHighlightBorderColor
                            }
                            return "transparent"
                        }
                    }

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.largeSpacing

                        Kirigami.Icon {
                            source: "configure"
                            implicitWidth: Kirigami.Units.iconSizes.small
                            implicitHeight: implicitWidth
                        }

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: i18n("Settings")
                            color: BreezeConstants.popupTextColor
                            elide: Text.ElideRight
                            font.family: BreezeConstants.fontFamily
                            font.weight: Font.Light
                        }
                    }

                    onClicked: {
                        root.showSettings()
                        root.close()
                    }
                }
            }
        }
    }
}
