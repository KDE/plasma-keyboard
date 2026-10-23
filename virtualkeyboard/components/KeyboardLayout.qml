/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard.virtualkeyboard

ColumnLayout {
    id: root

    /**
     * Default weight of keys.
     */
    property real defaultKeyWeight: 100

    /**
     * Default height weight of keyboard rows.
     */
    property real defaultRowHeightWeight: 100

    /**
     * Set a maximum width of the keyboard in relation to its height (so it doesn't stretch forever).
     */
    property real maxWidthToHeightRatio: -1

    /**
     * Multiplier for the keyboard panel height. Use this for layouts with extra rows.
     */
    property real panelHeightFactor: 1

    property string packageId: ""
    property string layoutId: ""
    property var virtualKeyboardContext: null
    readonly property var inputEngine: virtualKeyboardContext ? virtualKeyboardContext.inputEngine : VirtualKeyboard.inputEngine

    /**
     * Text composer to use while this layout is shown, instead of the one from the KeyboardLayoutDefinition (which is shared by all of its layout types).
     *
     * The active text composer is inputEngine.textComposer.
     */
    property QtObject textComposer: null

    Layout.fillWidth: true
    Layout.fillHeight: true
    width: parent ? parent.width : implicitWidth
    height: parent ? parent.height : implicitHeight
    spacing: 0
}
