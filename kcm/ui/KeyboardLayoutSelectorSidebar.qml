/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.bigscreen as Bigscreen

Bigscreen.SidebarOverlay {
    id: root

    openFocusItem: layoutList
    header: Bigscreen.SidebarOverlayHeader {
        title: i18n("Keyboard Layouts")
    }

    content: QQC2.ScrollView {
        ListView {
            id: layoutList
            Layout.fillWidth: true
            implicitHeight: contentHeight
            clip: true
            model: kcm.availableKeyboardLayouts

            delegate: Bigscreen.SwitchDelegate {
                id: layoutDelegate

                required property string layoutId
                required property string name
                required property string description

                width: layoutList.width
                text: layoutDelegate.name
                description: layoutDelegate.description
                checked: kcm.enabledKeyboardLayoutIds.includes(layoutDelegate.layoutId)

                onCheckedChanged: {
                    if (checked) {
                        kcm.enableKeyboardLayout(layoutDelegate.layoutId);
                    } else {
                        kcm.disableKeyboardLayout(layoutDelegate.layoutId);
                    }
                }
            }
        }
    }
}
