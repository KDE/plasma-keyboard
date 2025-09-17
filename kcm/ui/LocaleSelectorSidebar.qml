/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.bigscreen as Bigscreen

import org.kde.kitemmodels
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

Bigscreen.SidebarOverlay {
    id: root

    property alias model: langs.model

    openFocusItem: langs
    header: Bigscreen.SidebarOverlayHeader {
        title: i18n("Languages")
    }

    function updateModel() {
        let list = [];
        for (let locale of sourceModel) {
            const localeText = Qt.locale(locale).nativeLanguageName;
            if (searchText.length === 0 || localeText.toLowerCase().indexOf(searchText.toLowerCase()) !== -1) {
                list.push(locale);
            }
        }
        model = list;
    }

    onModelChanged: updateModel()

    Connections {
        target: VirtualKeyboardSettings

        function onAvailableLocalesChanged() {
            langs.model = VirtualKeyboardSettings.availableLocales;
        }
    }

    // HACK: needed to populate VirtualKeyboardSettings.availableLocales
    InputPanel {}

    content: QQC2.ScrollView {
        ListView {
            id: langs
            Layout.fillWidth: true
            implicitHeight: contentHeight
            clip: true

            delegate: Bigscreen.SwitchDelegate {
                id: delegate
                width: langs.width
                text: Qt.locale(modelData).nativeLanguageName
                onClicked: checked = !checked
                Keys.onReturnPressed: checked = !checked
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
    }
}
