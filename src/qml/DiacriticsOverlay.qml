/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents

/*!
The contents of an overlay that displays diacritic character options for the given base character.
*/
Item {
    id: root
    property string baseCharacter: ""
    property var options: []

    signal characterSelected(string character)

    visible: false
    z: 1000

    function show(base, choices) {
        baseCharacter = base;
        options = choices;
        visible = options.length > 0;
    }

    function close() {
        visible = false;
    }

    implicitWidth: background.implicitWidth
    implicitHeight: background.implicitHeight

    Rectangle {
        id: background
        anchors.centerIn: parent
        radius: Kirigami.Units.smallSpacing
        color: Kirigami.Theme.backgroundColor
        border.color: Kirigami.Theme.activeBackgroundColor
        border.width: 1
        visible: root.visible

        property int inset: Kirigami.Units.smallSpacing

        implicitWidth: row.implicitWidth + inset * 2
        implicitHeight: row.implicitHeight + inset * 2

        Row {
            id: row
            anchors.centerIn: parent
            spacing: Kirigami.Units.smallSpacing

            Repeater {
                model: root.options

                delegate: PlasmaComponents.Button {
                    id: delegate

                    required property string modelData
                    required property int index

                    flat: true
                    padding: Kirigami.Units.smallSpacing

                    onClicked: {
                        root.characterSelected(delegate.modelData)
                        root.close()
                    }

                    contentItem: Column {
                        spacing: Kirigami.Units.smallSpacing

                        Text {
                            text: delegate.modelData
                            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.2
                            font.weight: Font.Medium
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: Kirigami.Theme.textColor
                        }

                        Text {
                            text: delegate.index + 1
                            font.pointSize: Kirigami.Theme.smallFont.pointSize
                            color: Kirigami.Theme.disabledTextColor
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
            }
        }
    }
}
