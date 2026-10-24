/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    id: root

    title: i18n("On-Screen Keyboard Layouts")

    actions: [
        Kirigami.Action {
            id: addLayoutAction
            text: i18n("Add Layout…")
            icon.name: "list-add-symbolic"
            onTriggered: kcm.push("AddKeyboardLayoutPage.qml")
        }
    ]

    ListView {
        id: layoutList

        model: kcm.enabledKeyboardLayouts

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - Kirigami.Units.gridUnit * 4
            visible: layoutList.count === 0
            icon.name: "input-keyboard-virtual"
            text: i18n("No keyboard layouts selected")
            explanation: i18n("The default keyboard layout for the system will be used.")
            helpfulAction: addLayoutAction
        }

        delegate: QQC2.ItemDelegate {
            id: layoutDelegate

            required property int index
            required property string layoutId
            required property string name

            width: layoutList.width

            down: false
            highlighted: false
            hoverEnabled: false

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Kirigami.TitleSubtitle {
                    Layout.fillWidth: true
                    title: layoutDelegate.name
                }

                QQC2.ToolButton {
                    icon.name: "arrow-up-symbolic"
                    text: i18nc("Move the layout up a row", "Move Up")
                    display: QQC2.AbstractButton.IconOnly
                    visible: layoutList.count > 1
                    enabled: layoutDelegate.index > 0
                    onClicked: kcm.moveKeyboardLayout(layoutDelegate.index, layoutDelegate.index - 1)

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                }

                QQC2.ToolButton {
                    icon.name: "arrow-down-symbolic"
                    text: i18nc("Move the layout down a row", "Move Down")
                    display: QQC2.AbstractButton.IconOnly
                    visible: layoutList.count > 1
                    enabled: layoutDelegate.index < layoutList.count - 1
                    onClicked: kcm.moveKeyboardLayout(layoutDelegate.index, layoutDelegate.index + 1)

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                }

                QQC2.ToolButton {
                    icon.name: "list-remove-symbolic"
                    text: i18nc("Remove the layout", "Remove")
                    display: QQC2.AbstractButton.IconOnly
                    onClicked: kcm.disableKeyboardLayout(layoutDelegate.layoutId)

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                }
            }
        }
    }
}
