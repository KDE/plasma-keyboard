/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

Item {
    id: root

    property var languagePopup: null
    property var virtualKeyboardContext: null
    property real padding: 0
    property real leftPadding: padding
    property real rightPadding: padding
    property real topPadding: padding
    property real bottomPadding: padding

    readonly property var inputEngine: virtualKeyboardContext ? virtualKeyboardContext.inputEngine : VirtualKeyboard.inputEngine
    readonly property var keyboardController: virtualKeyboardContext ? virtualKeyboardContext.keyboardController : VirtualKeyboard.keyboardController
    readonly property var keyboardPackageResolver: virtualKeyboardContext ? virtualKeyboardContext.keyboardPackageResolver : VirtualKeyboard.keyboardPackageResolver
    property alias navigationModeActive: keyboardNavigation.navigationModeActive

    readonly property string layoutId: keyboardController ? keyboardController.layoutId : ""
    readonly property string packageId: keyboardPackageResolver ? keyboardPackageResolver.packageId(layoutId) : ""
    readonly property string layoutType: keyboardController ? keyboardController.layoutType : "main"

    implicitHeight: __keyboardHeight
    implicitWidth: __keyboardWidth

    // Always have the keyboard panel be 30% of the screen height, or 150px, whichever is larger
    readonly property real __baseKeyboardInputAreaHeight: Math.max(Screen.height * 0.3, 150)
    readonly property real __keyboardInputAreaHeight: __baseKeyboardInputAreaHeight * ((layoutLoader.item && layoutLoader.item.panelHeightFactor !== undefined) ? layoutLoader.item.panelHeightFactor : 1)
    readonly property real __contentHeight: __keyboardInputAreaHeight + (candidateStrip.visible ? candidateStrip.implicitHeight : 0)
    readonly property real __horizontalPadding: leftPadding + rightPadding
    readonly property real __verticalPadding: topPadding + bottomPadding
    readonly property real __keyboardHeight: __contentHeight + __verticalPadding
    readonly property real __keyboardWidth: Math.round(__contentHeight * aspectRatio) + __horizontalPadding

    // The value to multiply the height by to get the width
    readonly property real aspectRatio: {
        // Ratio to just fill the screen width
        const availableWidth = Math.max(0, Screen.width - __horizontalPadding);
        const fillScreenWidthRatio = availableWidth / __contentHeight;
        if (PlasmaKeyboardSettings.panelFillScreenWidth) {
            return fillScreenWidthRatio;
        }

        const targetAspectRatio = 3.0; // Target width ratio: 3 * height
        return Math.min(fillScreenWidthRatio, targetAspectRatio);
    }

    // The design ratios for the keyboard height
    readonly property real __keyboardDesignHeight: {
        if (Screen.width < 500) {
            // Phone mode
            return 800;
        }
        if (Screen.width < 1200) {
            // Wider
            return 600;
        }
        // Widest
        return 700;
    }
    readonly property real panelScaleHint: Math.max(0.3, __keyboardInputAreaHeight / __keyboardDesignHeight)

    function resetNavigation() {
        keyboardNavigation.resetNavigation();
    }

    function handleNavigationPressed(key) {
        keyboardNavigation.handleNavigationPressed(key);
    }

    function handleNavigationReleased(key) {
        keyboardNavigation.handleNavigationReleased(key);
    }

    function updateScaleHint() {
        if (width <= 0) {
            return;
        }
        BreezeConstants.scaleHint = panelScaleHint;
    }

    onPanelScaleHintChanged: updateScaleHint()
    Component.onCompleted: updateScaleHint()

    KeyboardNavigation {
        id: keyboardNavigation
        layoutLoader: layoutLoader
        keyboardStrip: candidateStrip
        languagePopup: root.languagePopup
    }

    ColumnLayout {
        anchors {
            fill: parent
            leftMargin: root.leftPadding
            rightMargin: root.rightPadding
            topMargin: root.topPadding
            bottomMargin: root.bottomPadding
        }
        spacing: 0

        KeyboardStrip {
            id: candidateStrip
            Layout.fillWidth: true
            inputEngine: root.inputEngine
        }

        Loader {
            id: layoutLoader

            Layout.preferredWidth: parent.width
            Layout.maximumWidth: (item && item.maxWidthToHeightRatio !== -1) ? (item.maxWidthToHeightRatio * height) : parent.width
            Layout.preferredHeight: root.__keyboardInputAreaHeight
            Layout.alignment: Qt.AlignHCenter

            source: root.keyboardPackageResolver ? root.keyboardPackageResolver.layoutUrl(root.layoutId, root.layoutType) : ""

            onLoaded: {
                resetNavigation();
                if (item && item.virtualKeyboardContext !== undefined) {
                    item.virtualKeyboardContext = root.virtualKeyboardContext;
                }
                if (item && item.keyboardLayoutId !== undefined) {
                    item.keyboardLayoutId = root.layoutId;
                }
                if (item && item.packageId !== undefined) {
                    item.packageId = root.packageId;
                }
            }

            onStatusChanged: {
                if (status === Loader.Error) {
                    console.warn("Failed to load keyboard layout", root.layoutId, root.layoutType, source);
                }
            }
        }
    }
}
