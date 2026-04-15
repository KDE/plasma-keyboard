// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

// Key for QML-only actions that don't send text or key events.

AbstractKey {
    id: root

    KeyMouseArea {
        keyItem: root

        onReleaseFinished: {
            root.trigger();
        }
    }
}
