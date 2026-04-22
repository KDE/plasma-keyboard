/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

InputPanelWindow {
    id: root
    height: Screen.height
    width: Screen.width
    color: 'transparent'

    interactiveRegion: Qt.rect(keyboardPanel.x,
                               keyboardPanel.y,
                               keyboardPanel.width,
                               keyboardPanel.height)

    property alias inputMethodConnection: inputMethodConnection
    property alias virtualKeyboardContext: inputMethodConnection.virtualKeyboardContext

    // Called by keys
    property alias alternativeKeysPopupItem: keyboardPanel.alternativeKeysPopup
    property alias flickPreviewPopupItem: keyboardPanel.flickPreviewPopup
    property alias languagePopupItem: keyboardPanel.languagePopup

    onVisibleChanged: {
        if (!visible) {
            keyboardPanel.inputPanel.navigationModeActive = false;
            if (keyboardPanel.languagePopup) {
                keyboardPanel.languagePopup.close();
            }
        }
    }

    InputMethodConnection {
        id: inputMethodConnection
        window: root
        keyboardNavigationActive: keyboardPanel.inputPanel.navigationModeActive
    }

    Connections {
        target: root.inputMethodConnection
        function onKeyNavigationPressed(key) {
            keyboardPanel.inputPanel.handleNavigationPressed(key);
        }
        function onKeyNavigationReleased(key) {
            keyboardPanel.inputPanel.handleNavigationReleased(key);
        }
    }

    // Unified overlay system for diacritics, emoji, text expansion, etc.
    OverlayWindow {
        id: overlayWindow
        controller: root.inputMethodConnection.overlayController
        onCandidateSelected: (index) => {
            Feedback.play(Feedback.SelectionCommit);
            root.inputMethodConnection.overlayController.commitCandidate(index);
        }
    }

    KeyboardPanel {
        id: keyboardPanel
        hostWindow: root
        virtualKeyboardContext: root.virtualKeyboardContext
        onShowSettingsRequested: root.showSettings();
    }
}
