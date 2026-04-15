/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard

Loader {
    id: root

    property string packageId: ""
    property string layoutId: ""
    property var virtualKeyboardContext: null
    readonly property var inputEngine: virtualKeyboardContext ? virtualKeyboardContext.inputEngine : VirtualKeyboard.inputEngine

    function applyLayoutProperties() {
        if (!item) {
            return;
        }
        if (item.virtualKeyboardContext !== undefined) {
            item.virtualKeyboardContext = virtualKeyboardContext;
        }
        if (item.layoutId !== undefined) {
            item.layoutId = layoutId;
        }
        if (item.packageId !== undefined) {
            item.packageId = packageId;
        }
    }

    onLoaded: applyLayoutProperties()
    onVirtualKeyboardContextChanged: applyLayoutProperties()
    onPackageIdChanged: applyLayoutProperties()
    onLayoutIdChanged: applyLayoutProperties()
}
