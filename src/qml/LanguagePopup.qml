/*
    SPDX-FileCopyrightText: 2025 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings
import QtQuick.Templates as T

import org.kde.layershell as LayerShell
import org.kde.kirigami as Kirigami
import org.kde.plasma.keyboard.lib as PlasmaKeyboard

T.Popup {
    id: root
    property var style
    property var keyboardPanel

    signal showSettings()

    width: languageListView.rowWidth + languageListView.margins * 2
    height: Math.min(languageListView.contentHeight + languageListView.spacing + settingsDelegate.implicitHeight + settingsDelegate.Layout.topMargin, languageListView.rowHeight * 6)
            + languageListView.margins * 2
    modal: true
    visible: false

    background: PlasmaKeyboard.BreezePopup {
        theme: root.style.theme
    }

    function show(parentItem, localeList, currentIndex) {
        languageListModel.clear()
        for (var i in localeList) {
            languageListModel.append({localeName: localeList[i], displayName: Qt.locale(localeList[i]).nativeLanguageName})
        }
        languageListView.currentIndex = currentIndex
        languageListView.positionViewAtIndex(currentIndex, ListView.Center)

        root.x = Qt.binding(() => keyboardPanel.mapFromItem(parentItem, 0, 0).x);
        root.y = Qt.binding(() => keyboardPanel.mapFromItem(parentItem, 0, -root.height).y);

        root.visible = true
    }

    TextMetrics {
        id: languageNameTextMetrics
        font {
            family: root.style.theme.fontFamily
            weight: Font.Light
            pixelSize: 44 * root.style.scaleHint
        }
        text: "X"
    }

    contentItem: Item {
        clip: true

        ColumnLayout {
            anchors.leftMargin: languageListView.margins
            anchors.rightMargin: languageListView.margins
            anchors.bottomMargin: languageListView.margins
            anchors.fill: parent
            spacing: 0

            ListView {
                id: languageListView
                clip: true
                topMargin: languageListView.margins // so clip goes to the edge
                bottomMargin: spacing
                Layout.fillWidth: true
                Layout.fillHeight: true

                readonly property real margins: 20 * style.scaleHint

                readonly property real rowWidth: languageNameTextMetrics.width * 17
                readonly property real rowHeight: languageNameTextMetrics.height + rowPadding * 2
                readonly property real rowPadding: languageNameTextMetrics.height / 4

                QQC2.ScrollBar.vertical: QQC2.ScrollBar {}
                model: ListModel {
                    id: languageListModel
                }

                spacing: root.style.scaleHint * 4

                delegate: LanguagePopupDelegate {
                    id: itemDelegate
                    style: root.style
                    text: displayName
                    width: languageListView.rowWidth
                    height: languageListView.rowHeight
                    padding: languageListView.rowPadding

                    onClicked: {
                        languageListView.currentIndex = index;
                        VirtualKeyboardSettings.locale = languageListModel.get(index).localeName;
                        root.close();
                    }
                }
            }

            Kirigami.Separator { Layout.fillWidth: true }
            LanguagePopupDelegate {
                id: settingsDelegate
                Layout.topMargin: root.style.scaleHint * 4
                Layout.fillWidth: true

                style: root.style
                implicitWidth: languageListView.rowWidth
                implicitHeight: languageListView.rowHeight
                padding: languageListView.rowPadding

                icon.name: "configure"
                text: i18n("Settings")
                onClicked: {
                    root.showSettings();
                    root.close();
                }
            }
        }
    }
}
