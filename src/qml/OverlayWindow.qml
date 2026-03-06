/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.plasma.keyboard

/**
 * Generic window for overlay popups.
 *
 * This window hosts overlay content (diacritics, emoji, text expansion) using
 * the Wayland overlay panel role for compositor-managed positioning.
 *
 * The content is loaded dynamically based on the active trigger type.
 */
InputPanelWindow {
    id: root

    /**
     * The overlay controller managing state and candidates.
     */
    required property OverlayController controller

    /**
     * Emitted when a candidate is selected.
     */
    signal candidateSelected(int index)

    visible: controller.overlayVisible
    color: "transparent"

    width: contentLoader.item ? contentLoader.item.implicitWidth : 100
    height: contentLoader.item ? contentLoader.item.implicitHeight : 50

    Component.onCompleted: {
        root.initInputPanel(InputPanelRole.OverlayPanel);
    }

    onWidthChanged: updateInteractiveRegion()
    onHeightChanged: updateInteractiveRegion()

    function updateInteractiveRegion() {
        root.interactiveRegion = Qt.rect(0, 0, root.width, root.height);
    }

    Loader {
        id: contentLoader
        anchors.fill: parent
        active: root.controller.overlayVisible

        // Force reload when trigger changes while overlay is visible
        property string currentTriggerId: root.controller.activeTriggerId
        onCurrentTriggerIdChanged: {
            if (active) {
                active = false;
                active = Qt.binding(() => root.controller.overlayVisible);
            }
        }

        sourceComponent: {
            switch (root.controller.activeTriggerId) {
            case "diacritics":
                return diacriticsViewComponent;
            case "emoji":
                return emojiViewComponent;
            case "textexpansion":
                return textExpansionViewComponent;
            default:
                return diacriticsViewComponent;
            }
        }

        onLoaded: {
            root.updateInteractiveRegion();
        }
    }

    // Diacritics view component
    Component {
        id: diacriticsViewComponent
        DiacriticsOverlay {
            id: diacriticsOverlay

            property var cachedOptions: []

            // Update options when the candidate model resets
            Connections {
                target: root.controller.candidateModel
                function onModelReset() {
                    const model = root.controller.candidateModel;
                    const opts = [];
                    for (let i = 0; i < model.rowCount(); i++) {
                        opts.push(model.insertTextAt(i));
                    }
                    diacriticsOverlay.cachedOptions = opts;
                }
            }

            // Initialize options when component completes
            Component.onCompleted: {
                const model = root.controller.candidateModel;
                const opts = [];
                for (let i = 0; i < model.rowCount(); i++) {
                    opts.push(model.insertTextAt(i));
                }
                cachedOptions = opts;
            }

            options: cachedOptions
            baseCharacter: root.controller.pendingText
            visible: true

            onCharacterSelected: character => {
                // Find index and commit
                for (let i = 0; i < cachedOptions.length; i++) {
                    if (cachedOptions[i] === character) {
                        root.candidateSelected(i);
                        return;
                    }
                }
            }
        }
    }

    // Emoji view component (stub)
    Component {
        id: emojiViewComponent
        Item {
            implicitWidth: 200
            implicitHeight: 200
            // TODO: Implement emoji list view
            Text {
                anchors.centerIn: parent
                text: "Emoji picker (not implemented)"
            }
        }
    }

    // Text expansion view component (stub)
    Component {
        id: textExpansionViewComponent
        Item {
            implicitWidth: 200
            implicitHeight: 100
            // Text expansion typically doesn't show a popup
            // This is a placeholder, for potential preview UI or candidate list maybe? Maybe not.
            Text {
                anchors.centerIn: parent
                text: "Text expansion preview"
            }
        }
    }
}
