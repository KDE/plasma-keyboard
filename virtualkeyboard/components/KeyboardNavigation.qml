/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard

QtObject {
    id: root

    enum ActivationState {
        Idle,
        KeyPressed,
        PopupWaitingForRelease,
        PopupOpen,
        PopupConfirmPressed,
        PopupEscapePressed
    }

    required property Loader layoutLoader
    required property var keyboardStrip
    property var languagePopup: null

    readonly property bool navigationAvailable: layoutLoader.status === Loader.Ready && layoutLoader.item !== null && navigableKeys().length > 0
    property bool navigationModeActive: false
    property var navigationKeyItem: null
    property string navigationArea: ""

    // Track Enter and alternate popup state
    property int __activationState: KeyboardNavigation.ActivationState.Idle
    property var __activationKeyItem: null
    readonly property Timer __activationHoldTimer: Timer {
        interval: 500
        repeat: false
        onTriggered: root.openAlternativePopup()
    }

    function clearNavigationKey() {
        if (navigationKeyItem) {
            navigationKeyItem.navigationActive = false;
        }
        navigationKeyItem = null;
    }

    function setNavigationKey(item) {
        if (navigationKeyItem === item) {
            return;
        }
        clearNavigationKey();
        navigationKeyItem = item;
        if (navigationKeyItem) {
            navigationKeyItem.navigationActive = true;
        }
    }

    function resetNavigation() {
        cancelNavigationActivation();
        clearNavigationKey();
        navigationArea = "";
        navigationModeActive = false;
        keyboardStrip.navigationModeActive = false;
        keyboardStrip.resetNavigation();
    }

    function collectNavigableKeys(item, keys) {
        if (!item || !item.visible) {
            return;
        }

        if (item.navigationActive !== undefined && item.triggerByNavigation !== undefined && item.enabled !== false) {
            keys.push(item);
        }

        for (let i = 0; i < item.children.length; ++i) {
            collectNavigableKeys(item.children[i], keys);
        }
    }

    function navigableKeys() {
        const keys = [];
        if (layoutLoader.item) {
            collectNavigableKeys(layoutLoader.item, keys);
        }
        return keys;
    }

    function keyCenter(item) {
        return item.mapToItem(layoutLoader, item.width / 2, item.height / 2);
    }

    function findHorizontalKey(direction) {
        // Move left and right in reading order
        const keys = navigableKeys();
        if (keys.length === 0) {
            return null;
        }

        const currentIndex = keys.indexOf(navigationKeyItem);
        if (currentIndex === -1) {
            return keys[0];
        }
        return keys[(currentIndex + direction + keys.length) % keys.length];
    }

    function findVerticalKey(direction) {
        // Move to the closest key in the nearest row
        const keys = navigableKeys();
        if (keys.length === 0) {
            return null;
        }

        if (!navigationKeyItem || direction === 0) {
            return keys[0];
        }

        const currentCenter = keyCenter(navigationKeyItem);
        let closestKey = null;
        let closestVerticalDistance = Number.MAX_VALUE;
        let closestHorizontalDistance = Number.MAX_VALUE;
        for (let i = 0; i < keys.length; ++i) {
            const key = keys[i];
            if (key === navigationKeyItem) {
                continue;
            }

            const center = keyCenter(key);
            const verticalDistance = (center.y - currentCenter.y) * direction;
            const sameRowThreshold = Math.min(key.height, navigationKeyItem.height) / 2;
            if (verticalDistance <= sameRowThreshold) {
                continue;
            }

            const horizontalDistance = Math.abs(center.x - currentCenter.x);
            if (verticalDistance < closestVerticalDistance
                    || (verticalDistance === closestVerticalDistance && horizontalDistance < closestHorizontalDistance)) {
                closestKey = key;
                closestVerticalDistance = verticalDistance;
                closestHorizontalDistance = horizontalDistance;
            }
        }
        return closestKey;
    }

    function moveKeyNavigation(dx, dy) {
        const nextKey = dx !== 0 ? findHorizontalKey(dx) : findVerticalKey(dy);
        if (!nextKey) {
            return false;
        }

        setNavigationKey(nextKey);
        navigationArea = "keys";
        navigationModeActive = true;
        keyboardStrip.navigationModeActive = false;
        keyboardStrip.resetNavigation();
        return true;
    }

    function startNavigationActivation() {
        // Start Enter activation and long press detection
        if (__activationState !== KeyboardNavigation.ActivationState.Idle) {
            return;
        }

        __activationState = KeyboardNavigation.ActivationState.KeyPressed;
        __activationKeyItem = navigationArea === "keys" ? navigationKeyItem : null;
        if (!__activationKeyItem) {
            return;
        }

        __activationKeyItem.pressedVisual = true;
        if (__activationKeyItem.effectiveAlternativeKeys && __activationKeyItem.effectiveAlternativeKeys.length > 0) {
            __activationHoldTimer.restart();
        }
    }

    function openAlternativePopup() {
        const popup = VirtualKeyboard.alternativeKeysPopup;
        if (__activationState !== KeyboardNavigation.ActivationState.KeyPressed
                || !__activationKeyItem
                || __activationKeyItem !== navigationKeyItem
                || !popup) {
            return;
        }
        if (popup.openForKey(__activationKeyItem)) {
            __activationState = KeyboardNavigation.ActivationState.PopupWaitingForRelease;
        }
    }

    function cancelNavigationActivation() {
        __activationHoldTimer.stop();
        if (__activationKeyItem) {
            __activationKeyItem.pressedVisual = false;
        }
        const popup = VirtualKeyboard.alternativeKeysPopup;
        if (popup && popup.active && popup.ownerKey === __activationKeyItem) {
            popup.close();
        }
        __activationState = KeyboardNavigation.ActivationState.Idle;
        __activationKeyItem = null;
    }

    function focusCandidates() {
        if (!keyboardStrip.visible || !keyboardStrip.ensureNavigationSelection(false)) {
            return false;
        }

        clearNavigationKey();
        navigationArea = "candidates";
        navigationModeActive = true;
        keyboardStrip.navigationModeActive = true;
        return true;
    }

    function moveHorizontal(direction) {
        if (navigationArea !== "candidates" || !keyboardStrip.moveSelection(direction)) {
            moveKeyNavigation(direction, 0);
        }
    }

    function handleNavigationPressed(key) {
        // Handle alternate popup navigation first
        const alternativePopup = VirtualKeyboard.alternativeKeysPopup;
        if (alternativePopup && alternativePopup.active && alternativePopup.ownerKey === __activationKeyItem) {
            if (key === Qt.Key_Left) {
                alternativePopup.moveSelection(-1);
            } else if (key === Qt.Key_Right) {
                alternativePopup.moveSelection(1);
            } else if (key === Qt.Key_Escape) {
                alternativePopup.close();
                __activationState = KeyboardNavigation.ActivationState.PopupEscapePressed;
            } else if ((key === Qt.Key_Return || key === Qt.Key_Enter)
                    && __activationState === KeyboardNavigation.ActivationState.PopupOpen) {
                __activationState = KeyboardNavigation.ActivationState.PopupConfirmPressed;
            }
            return;
        }

        if (languagePopup && languagePopup.popupVisible) {
            navigationModeActive = true;
            if (key === Qt.Key_Escape) {
                return;
            } else if (key === Qt.Key_Up) {
                languagePopup.moveSelection(-1);
            } else if (key === Qt.Key_Down) {
                languagePopup.moveSelection(1);
            } else if (key === Qt.Key_Left || key === Qt.Key_Right) {
                languagePopup.close();
                resetNavigation();
            }
            return;
        }

        switch (key) {
        case Qt.Key_Escape:
            break;
        case Qt.Key_Left:
            moveHorizontal(-1);
            break;
        case Qt.Key_Right:
            moveHorizontal(1);
            break;
        case Qt.Key_Up:
            if (navigationArea === "candidates") {
                break;
            }
            if (!moveKeyNavigation(0, -1) && keyboardStrip.visible) {
                focusCandidates();
            }
            break;
        case Qt.Key_Down:
            moveKeyNavigation(0, 1);
            break;
        case Qt.Key_Return:
        case Qt.Key_Enter:
            startNavigationActivation();
            break;
        default:
            break;
        }
    }

    function handleNavigationReleased(key) {
        if (key === Qt.Key_Escape) {
            if (__activationState === KeyboardNavigation.ActivationState.PopupEscapePressed) {
                __activationState = KeyboardNavigation.ActivationState.Idle;
                __activationKeyItem = null;
                return;
            }
            if (languagePopup && languagePopup.popupVisible) {
                languagePopup.close();
            }
            resetNavigation();
            return;
        }

        if (key !== Qt.Key_Return && key !== Qt.Key_Enter) {
            return;
        }

        __activationHoldTimer.stop();
        if (__activationKeyItem) {
            __activationKeyItem.pressedVisual = false;
        }

        if (__activationState === KeyboardNavigation.ActivationState.PopupWaitingForRelease) {
            // Keep the popup open after the long press release
            __activationState = KeyboardNavigation.ActivationState.PopupOpen;
            return;
        }
        if (__activationState === KeyboardNavigation.ActivationState.PopupConfirmPressed) {
            const alternativePopup = VirtualKeyboard.alternativeKeysPopup;
            if (alternativePopup && alternativePopup.active) {
                // Confirm the alternate on the next Enter release
                alternativePopup.commitCurrent();
            }
            __activationState = KeyboardNavigation.ActivationState.Idle;
            __activationKeyItem = null;
            return;
        }

        __activationState = KeyboardNavigation.ActivationState.Idle;

        if (languagePopup && languagePopup.popupVisible) {
            languagePopup.activateCurrent();
            return;
        }

        if (!navigationModeActive) {
            return;
        }

        if (navigationArea === "candidates") {
            keyboardStrip.activateCurrent();
            return;
        }

        if (!navigationKeyItem) {
            moveKeyNavigation(0, 0);
        }
        const keyItem = __activationKeyItem || navigationKeyItem;
        __activationKeyItem = null;
        if (keyItem) {
            keyItem.triggerByNavigation();
        }
    }
}
