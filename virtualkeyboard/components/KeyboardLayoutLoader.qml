/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick

Loader {
    id: root

    property string keyboardLayoutId: ""
    property string packageId: ""
    property var virtualKeyboardContext: null
    readonly property var inputEngine: virtualKeyboardContext ? virtualKeyboardContext.inputEngine : VirtualKeyboard.inputEngine

    function applyLayoutProperties() {
        if (!item) {
            return;
        }
        if (item.virtualKeyboardContext !== undefined) {
            item.virtualKeyboardContext = virtualKeyboardContext;
        }
        if (item.keyboardLayoutId !== undefined) {
            item.keyboardLayoutId = keyboardLayoutId;
        }
        if (item.packageId !== undefined) {
            item.packageId = packageId;
        }
    }

    onLoaded: applyLayoutProperties()
    onVirtualKeyboardContextChanged: applyLayoutProperties()
    onKeyboardLayoutIdChanged: applyLayoutProperties()
    onPackageIdChanged: applyLayoutProperties()
}
