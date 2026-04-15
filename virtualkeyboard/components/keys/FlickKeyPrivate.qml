// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

pragma Singleton

import QtQuick

QtObject {
    property var activeTapSequenceKey: null

    function keyPressStarted(key) {
        if (activeTapSequenceKey && activeTapSequenceKey !== key) {
            activeTapSequenceKey.__clearTapSequence();
            activeTapSequenceKey = null;
        }
    }

    function setActiveTapSequenceKey(key) {
        activeTapSequenceKey = key;
    }

    function clearActiveTapSequenceKey(key) {
        if (activeTapSequenceKey === key) {
            activeTapSequenceKey = null;
        }
    }
}
