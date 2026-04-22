/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

Kirigami.ShadowedRectangle {
    id: root

    property var hostWindow: null
    property var virtualKeyboardContext: null
    property alias inputPanel: inputPanel
    property alias languagePopup: languagePopup
    property alias alternativeKeysPopup: alternativeKeysPopup
    property alias flickPreviewPopup: flickPreviewPopup
    readonly property var keyboardController: virtualKeyboardContext ? virtualKeyboardContext.keyboardController : null

    readonly property bool isFloating: false
    readonly property bool isFullScreenWidth: !isFloating && PlasmaKeyboardSettings.panelFillScreenWidth

    signal showSettingsRequested()

    color: BreezeConstants.keyboardBackgroundColor

    corners {
        bottomLeftRadius: isFloating ? Kirigami.Units.cornerRadius : 0
        bottomRightRadius: isFloating ? Kirigami.Units.cornerRadius : 0
        topLeftRadius: isFullScreenWidth ? 0 : Kirigami.Units.cornerRadius
        topRightRadius: isFullScreenWidth ? 0 : Kirigami.Units.cornerRadius
    }

    shadow {
        size: isFullScreenWidth ? 0 : 16
        color: Qt.rgba(0, 0, 0, 0.3)
    }

    x: hostWindow ? (hostWindow.width / 2) - (width / 2) : 0
    y: hostWindow ? (hostWindow.height - height) : 0
    width: inputPanel.width > 0 ? inputPanel.width : 100
    height: inputPanel.height > 0 ? inputPanel.height : 100

    LanguagePopup {
        id: languagePopup
        keyboardPanel: inputPanel
        keyboardController: root.keyboardController
        onShowSettings: root.showSettingsRequested()
    }

    AlternativeKeysPopup {
        id: alternativeKeysPopup
        keyboardPanel: inputPanel
    }

    FlickPreviewPopup {
        id: flickPreviewPopup
    }

    PreeditBubble {
        anchors.left: inputPanel.left
        anchors.top: inputPanel.top
        anchors.topMargin: -implicitHeight - Math.round(8 * BreezeConstants.scaleHint)
        inputEngine: root.virtualKeyboardContext ? root.virtualKeyboardContext.inputEngine : null
        z: 2
    }

    Rectangle {
        id: topSeparator
        height: 1
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        opacity: 0.5
        color: BreezeConstants.primaryDarkColor
        visible: !isFloating && isFullScreenWidth
    }

    Keyboard {
        id: inputPanel
        languagePopup: languagePopup
        virtualKeyboardContext: root.virtualKeyboardContext

        padding: (!isFloating && isFullScreenWidth) ? Kirigami.Units.smallSpacing / 2 : Kirigami.Units.largeSpacing

        anchors {
            top: parent.top
            left: parent.left
        }

        function updateKeyboardLayouts() {
            if (!root.keyboardController) {
                return;
            }

            let layoutIds = PlasmaKeyboardSettings.enabledKeyboardLayoutIds;
            if (layoutIds.length === 0 && PlasmaKeyboardSettings.enabledLocales.length > 0) {
                layoutIds = root.keyboardController.layoutIdsForLocales(PlasmaKeyboardSettings.enabledLocales);
            }
            root.keyboardController.activeLayoutIds = layoutIds;
        }

        Connections {
            target: PlasmaKeyboardSettings
            function onEnabledKeyboardLayoutIdsChanged() {
                inputPanel.updateKeyboardLayouts();
            }
            function onEnabledLocalesChanged() {
                inputPanel.updateKeyboardLayouts();
            }
        }

        Component.onCompleted: {
            inputPanel.updateKeyboardLayouts();
        }
    }
}
