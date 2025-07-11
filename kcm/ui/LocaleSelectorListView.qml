/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

import org.kde.kitemmodels
import org.kde.kirigami as Kirigami

ListView {
    id: root

    property var sourceModel: []
    property string searchText: ''

    model: sourceModel

    function updateModel() {
        let list = [];
        for (let locale of sourceModel) {
            const localeText = Qt.locale(locale).nativeLanguageName;
            if (searchText.length === 0 || localeText.toLowerCase().indexOf(searchText.toLowerCase()) !== -1) {
                list.push(locale)
            }
        }
        model = list;
    }
    onSearchTextChanged: updateModel()
    onSourceModelChanged: updateModel()

    Connections {
        target: VirtualKeyboardSettings

        function onAvailableLocalesChanged() {
            root.sourceModel = VirtualKeyboardSettings.availableLocales;
        }
    }

    // HACK: needed to populate VirtualKeyboardSettings.availableLocales
    InputPanel {}

    headerPositioning: ListView.OverlayHeader
    header: QQC2.ToolBar {
        z: 999 // On top of content
        position: QQC2.ToolBar.header

        width: parent.width
        topPadding: Kirigami.Units.largeSpacing
        bottomPadding: Kirigami.Units.largeSpacing
        leftPadding: Kirigami.Units.largeSpacing
        rightPadding: Kirigami.Units.largeSpacing

        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        contentItem: Kirigami.SearchField {
            id: searchField
            onTextChanged: {
                root.searchText = text;
                searchField.forceActiveFocus();
            }
        }
    }

    delegate: QQC2.CheckDelegate {
        width: root.width
        text: Qt.locale(modelData).nativeLanguageName
        checked: kcm.enabledLocales.includes(modelData)
        onCheckedChanged: {
            if (checked) {
                kcm.enableLocale(modelData);
            } else {
                kcm.disableLocale(modelData);
            }
        }
    }
}
