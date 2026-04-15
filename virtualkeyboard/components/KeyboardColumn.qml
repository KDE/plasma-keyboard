/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root

    /**
     * Default width weight used by child keys that do not set their own
     * weight. By default it is inherited from the parent.
     */
    property real defaultKeyWeight: parent && parent.defaultKeyWeight !== undefined ? parent.defaultKeyWeight : 100

    /**
     * Width weight for this column when it is placed inside a KeyboardRow.
     * This lets a vertical stack of keys occupy the same horizontal slot as a
     * normal key.
     */
    property real weight: parent && parent.defaultKeyWeight !== undefined ? parent.defaultKeyWeight : 100

    // HACK: Layout.preferredWidth is used as a sort of size hint, not an exact size.
    // If the weight is too large, preferredWidth gets capped out and stuff starts breaking.
    Layout.preferredWidth: weight / Math.max(defaultKeyWeight, 1)
    Layout.minimumWidth: 0
    Layout.fillWidth: true
    Layout.fillHeight: true
    spacing: 0
}
