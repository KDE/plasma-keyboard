/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick

QtObject {
    id: root

    required property Loader layoutLoader
    required property var keyboardStrip
    property var languagePopup: null

    property bool navigationModeActive: false
    property var navigationKeyItem: null
    property string navigationArea: ""

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

    function initialNavigationPoint(dx, dy) {
        if (dx > 0) {
            return Qt.point(0, layoutLoader.height / 2);
        }
        if (dx < 0) {
            return Qt.point(layoutLoader.width, layoutLoader.height / 2);
        }
        if (dy > 0) {
            return Qt.point(layoutLoader.width / 2, 0);
        }
        if (dy < 0) {
            return Qt.point(layoutLoader.width / 2, layoutLoader.height);
        }
        return Qt.point(layoutLoader.width / 2, layoutLoader.height / 2);
    }

    function findDirectionalKey(dx, dy, wrap) {
        const keys = navigableKeys();
        if (keys.length === 0) {
            return null;
        }

        const currentPoint = navigationKeyItem ? keyCenter(navigationKeyItem) : initialNavigationPoint(dx, dy);
        let bestKey = null;
        let bestScore = Number.MAX_VALUE;

        for (let i = 0; i < keys.length; ++i) {
            const key = keys[i];
            if (key === navigationKeyItem) {
                continue;
            }

            const center = keyCenter(key);
            const deltaX = center.x - currentPoint.x;
            const deltaY = center.y - currentPoint.y;

            if (!wrap) {
                if (dx > 0 && deltaX <= 0) {
                    continue;
                }
                if (dx < 0 && deltaX >= 0) {
                    continue;
                }
                if (dy > 0 && deltaY <= 0) {
                    continue;
                }
                if (dy < 0 && deltaY >= 0) {
                    continue;
                }
            }

            let score = 0;
            if (dx !== 0) {
                const axis = wrap ? Math.abs(deltaX) : Math.max(1, Math.abs(deltaX));
                score = axis * axis * 4 + deltaY * deltaY;
            } else if (dy !== 0) {
                const axis = wrap ? Math.abs(deltaY) : Math.max(1, Math.abs(deltaY));
                score = axis * axis * 4 + deltaX * deltaX;
            } else {
                score = deltaX * deltaX + deltaY * deltaY;
            }

            if (score < bestScore) {
                bestScore = score;
                bestKey = key;
            }
        }

        return bestKey;
    }

    function moveKeyNavigation(dx, dy) {
        let nextKey = findDirectionalKey(dx, dy, false);
        if (!nextKey) {
            nextKey = findDirectionalKey(dx, dy, true);
        }
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

    function focusCandidates(fromRight) {
        if (!keyboardStrip.visible || !keyboardStrip.ensureNavigationSelection(fromRight)) {
            return false;
        }

        clearNavigationKey();
        navigationArea = "candidates";
        navigationModeActive = true;
        keyboardStrip.navigationModeActive = true;
        return true;
    }

    function handleNavigationPressed(key) {
        if (languagePopup && languagePopup.popupVisible) {
            navigationModeActive = true;
            if (key === Qt.Key_Up) {
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
        case Qt.Key_Left:
            if (navigationArea === "candidates") {
                if (!keyboardStrip.moveSelection(-1)) {
                    moveKeyNavigation(-1, 0);
                }
            } else {
                moveKeyNavigation(-1, 0);
            }
            break;
        case Qt.Key_Right:
            if (navigationArea === "candidates") {
                if (!keyboardStrip.moveSelection(1)) {
                    moveKeyNavigation(1, 0);
                }
            } else {
                moveKeyNavigation(1, 0);
            }
            break;
        case Qt.Key_Up:
            if (navigationArea === "candidates") {
                break;
            }
            if (!moveKeyNavigation(0, -1) && keyboardStrip.visible) {
                focusCandidates(false);
            }
            break;
        case Qt.Key_Down:
            moveKeyNavigation(0, 1);
            break;
        case Qt.Key_Return:
        case Qt.Key_Enter:
            if (!navigationModeActive) {
                if (navigationArea === "" && !navigationKeyItem) {
                    moveKeyNavigation(0, 0);
                }
                navigationModeActive = navigationArea !== "" || navigationKeyItem !== null;
            }
            break;
        default:
            break;
        }
    }

    function handleNavigationReleased(key) {
        if (key !== Qt.Key_Return && key !== Qt.Key_Enter) {
            return;
        }

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
        if (navigationKeyItem) {
            navigationKeyItem.triggerByNavigation();
        }
    }
}
