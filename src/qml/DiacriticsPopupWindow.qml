/*
    SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick

import org.kde.plasma.keyboard

InputPanelWindow {
    id: root

    signal characterSelected(string character)

    visible: popup.visible
    color: "transparent"

    width: popup.implicitWidth
    height: popup.implicitHeight

    Component.onCompleted: {
        root.initWaylandInputPanel(true)
    }

    function show(baseCharacter, choices) {
        popup.show(baseCharacter, choices)
        // Make the whole popup interactive; the window is already the right size.
        root.interactiveRegion = Qt.rect(0, 0, root.width, root.height)
    }

    function close() {
        popup.close()
    }

    onWidthChanged: root.interactiveRegion = Qt.rect(0, 0, root.width, root.height)
    onHeightChanged: root.interactiveRegion = Qt.rect(0, 0, root.width, root.height)

    DiacriticsPopup {
        id: popup
        anchors.fill: parent
        onCharacterSelected: (character) => root.characterSelected(character)
    }
}
