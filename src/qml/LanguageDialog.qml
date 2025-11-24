/*
    SPDX-FileCopyrightText: 2025 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings
import org.kde.layershell as LayerShell
import org.kde.kirigami as Kirigami

Kirigami.ApplicationWindow {
    id: languageDialog
    title: i18nc("@title", "Switch Language")

    signal showSettings();

    visible: false
    height: Screen.height / 3

    LayerShell.Window.anchors: LayerShell.Window.AnchorBottom | LayerShell.Window.AnchorLeft | LayerShell.Window.AnchorRight
    LayerShell.Window.layer: LayerShell.Window.LayerTop
    LayerShell.Window.keyboardInteractivity: LayerShell.Window.KeyboardInteractivityNone

    function show(localeList, currentIndex) {
        languageListModel.clear()
        for (var i in localeList) {
            languageListModel.append({localeName: localeList[i], displayName: Qt.locale(localeList[i]).nativeLanguageName})
        }
        languageListView.currentIndex = currentIndex
        languageListView.positionViewAtIndex(currentIndex, ListView.Center)
        languageDialog.visible = true
    }

    pageStack.initialPage: Kirigami.ScrollablePage {
        title: i18nc("@title", "Languages")
        actions: [
            Action {
                icon.name: "configure"
                onTriggered: languageDialog.showSettings()
            }
        ]
        ListView {
            id: languageListView
            anchors.fill: parent
            ScrollBar.horizontal: ScrollBar {}
            ScrollBar.vertical: ScrollBar {}
            model: ListModel {
                id: languageListModel
            }
            delegate: ItemDelegate {
                id: languageListItem
                text: displayName
                width: parent.width
                onClicked: {
                    languageListView.currentIndex = index
                    VirtualKeyboardSettings.locale = languageListModel.get(index).localeName
                    languageDialog.close()
                }
            }
        }
    }
}
