/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.plasma.keyboard

/*!
A window containing a diacritics popup for character selection.

TODO: .. make generic for also e.g. emoji picker, clipboard history, etc?
*/
InputPanelWindow {
    id: root

    signal characterSelected(string character)

    visible: contents.visible
    color: "transparent"

    width: contents.implicitWidth
    height: contents.implicitHeight

    Component.onCompleted: {
        root.initInputPanel(InputPanelRole.OverlayPanel)
    }

    function show(baseCharacter, choices) {
        contents.show(baseCharacter, choices)
        // Make the whole popup interactive; the window is already the right size.
        root.interactiveRegion = Qt.rect(0, 0, root.width, root.height)
    }

    function close() {
        contents.close()
    }

    onWidthChanged: root.interactiveRegion = Qt.rect(0, 0, root.width, root.height)
    onHeightChanged: root.interactiveRegion = Qt.rect(0, 0, root.width, root.height)

    DiacriticsOverlay {
        id: contents
        anchors.fill: parent
        onCharacterSelected: (character) => root.characterSelected(character)
    }
}
