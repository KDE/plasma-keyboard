/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

ListView {
    id: root

    model: kcm.availableKeyboardLayoutGroups

    headerPositioning: ListView.OverlayHeader
    header: QQC2.ToolBar {
        width: parent.width
        z: 999
        position: QQC2.ToolBar.Header

        topPadding: Kirigami.Units.largeSpacing
        bottomPadding: Kirigami.Units.largeSpacing
        leftPadding: Kirigami.Units.largeSpacing
        rightPadding: Kirigami.Units.largeSpacing

        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        contentItem: ColumnLayout {
            spacing: 0

            Kirigami.SearchField {
                id: searchField
                placeholderText: i18n("Filter languages…")
                Accessible.name: i18n("Filter languages")

                Layout.fillWidth: true

                onTextChanged: {
                    kcm.availableKeyboardLayoutGroups.filterText = text;
                    searchField.forceActiveFocus();
                }
            }

            QQC2.CheckBox {
                Layout.topMargin: Kirigami.Units.smallSpacing

                text: i18nc("@option:check", "Show all layouts")
                checked: !kcm.keyboardLayoutFormFactorFilterEnabled

                Layout.fillWidth: true

                onCheckedChanged: {
                    kcm.keyboardLayoutFormFactorFilterEnabled = !checked;
                    checked = Qt.binding(() => !kcm.keyboardLayoutFormFactorFilterEnabled);
                }
            }

            Kirigami.InlineMessage {
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.fillWidth: true
                text: i18n("No keyboard layouts selected. The default keyboard layout for the system will be used.")
                type: Kirigami.MessageType.Information
                visible: kcm.enabledKeyboardLayoutIds.length === 0
            }
        }
    }

    delegate: ColumnLayout {
        id: groupDelegate

        required property string groupId
        required property string name
        required property string description
        required property var layouts
        required property bool matchesFilter
        required property bool displayExpanded
        required property int selectedCount
        required property int visibleLayoutCount

        width: root.width
        height: matchesFilter ? implicitHeight : 0
        visible: matchesFilter
        spacing: 0

        QQC2.ItemDelegate {
            Layout.fillWidth: true
            text: groupDelegate.name
            onClicked: kcm.availableKeyboardLayoutGroups.toggleExpanded(groupDelegate.groupId)

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    source: displayExpanded ? "go-down-symbolic" : "go-next-symbolic"
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: implicitWidth
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Kirigami.Units.smallSpacing

                    QQC2.Label {
                        Layout.fillWidth: true
                        text: groupDelegate.name
                        elide: Text.ElideRight
                    }

                    QQC2.Label {
                        Layout.fillWidth: true
                        text: {
                            const selectionText = selectedCount === 0
                                ? i18np("%1 layout", "%1 layouts", visibleLayoutCount)
                                : i18n("%1 of %2 selected", selectedCount, visibleLayoutCount);
                            return selectionText;
                        }
                        elide: Text.ElideRight
                        opacity: 0.7
                        font: Kirigami.Theme.smallFont
                    }
                }
            }
        }

        ColumnLayout {
            visible: displayExpanded
            Layout.fillWidth: true
            spacing: 0

            Repeater {
                model: groupDelegate.layouts

                delegate: QQC2.CheckDelegate {
                    id: layoutDelegate

                    required property string layoutId
                    required property string name
                    required property string description
                    required property bool matchesFilter
                    required property bool enabled
                    Layout.fillWidth: true
                    visible: matchesFilter
                    height: matchesFilter ? implicitHeight : 0
                    leftPadding: Kirigami.Units.gridUnit * 2
                    text: layoutDelegate.name
                    checked: layoutDelegate.enabled

                    contentItem: ColumnLayout {
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: layoutDelegate.name
                            elide: Text.ElideRight
                        }

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: layoutDelegate.description
                            visible: text.length > 0
                            elide: Text.ElideRight
                            opacity: 0.7
                            font: Kirigami.Theme.smallFont
                        }
                    }

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
}
