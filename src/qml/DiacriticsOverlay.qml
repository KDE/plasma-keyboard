/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.kirigami as Kirigami

/*!
The contents of an overlay that displays diacritic character options for the given base character.

TODO: etc, for example, blah blah.
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
                delegate: Button {
                    required property string modelData
                    text: modelData
                    padding: Kirigami.Units.smallSpacing
                    onClicked: {
                        root.characterSelected(modelData)
                        root.close()
                    }
                }
            }
        }
    }
}
