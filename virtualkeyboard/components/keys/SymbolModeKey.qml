// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.plasma.keyboard.virtualkeyboard

// Action key that switches between the main and symbol layouts.

ActionKey {
    id: root

    displayText: "?123"
    functionKey: true
    secondaryStyle: true

    onClicked: VirtualKeyboard.keyboardController.symbolMode = !VirtualKeyboard.keyboardController.symbolMode
}
