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
    property int selectedIndex: -1

    signal characterSelected(string character)

    visible: false
    z: 1000

    function show(base, choices) {
        baseCharacter = base;
        options = choices;
        selectedIndex = -1;
        visible = options.length > 0;
    }

    function close() {
        visible = false;
    }

    // Handle arrow keys to navigate options and Enter to select. This allows the overlay to be
    // fully keyboard-navigable.
    //
    // The first key press after the overlay opens will select the first option (index 0).
    // Subsequent left/right keys will move the selection, while up/down will jump to the
    // first/last option.
    function handleNavigationKey(key) {
        switch (key) {
        case Qt.Key_Right:
            if (root.selectedIndex === -1) {
                root.selectedIndex = 0;
            } else if (root.selectedIndex < root.options.length - 1) {
                root.selectedIndex++;
            }
            break;
        case Qt.Key_Left:
            if (root.selectedIndex === -1) {
                root.selectedIndex = 0;
            } else if (root.selectedIndex > 0) {
                root.selectedIndex--;
            }
            break;
        case Qt.Key_Up:
            root.selectedIndex = 0;
            break;
        case Qt.Key_Down:
            if (root.selectedIndex === -1) {
                root.selectedIndex = 0;
            } else {
                root.selectedIndex = root.options.length - 1;
            }
            break;
        case Qt.Key_Return:
            if (root.selectedIndex >= 0 && root.selectedIndex < root.options.length) {
                root.characterSelected(root.options[root.selectedIndex]);
                root.close();
            } else {
                // If no option is selected, close the overlay without selecting a character
                root.close();
            }
            break;
        }
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

                    // Highlight the selected index (if any) to provide visual feedback when
                    // navigating with arrow keys.
                    down: delegate.index === root.selectedIndex

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
                            // Only show number hints for the first 9 candidates
                            // (keys 1–9). Candidates beyond index 8 can still be
                            // selected by tapping but have no keyboard shortcut.
                            //
                            // We use opacity rather than visible so even options without a number
                            // hint still have the same layout / selection appearance as those with hints.
                            opacity: delegate.index < 9 ? 1 : 0
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
