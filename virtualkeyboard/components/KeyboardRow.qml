/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts

RowLayout {
    id: root

    /**
     * Default width weight used by child keys that do not set their own
     * weight. By default it is inherited from the parent.
     */
    property real defaultKeyWeight: parent && parent.defaultKeyWeight !== undefined ? parent.defaultKeyWeight : 100

    /**
     * Width weight for this row when it is nested inside another KeyboardRow.
     * Nested rows can use it to occupy the same horizontal slot as a key.
     */
    property real weight: parent && parent.defaultKeyWeight !== undefined ? parent.defaultKeyWeight : 100

    /**
     * Relative height weight used by the parent KeyboardLayout when
     * distributing vertical space between rows.
     * For example, use heightWeight: defaultRowHeightWeight / 2 for a
     * half-height row.
     */
    property real heightWeight: parent && parent.defaultRowHeightWeight !== undefined ? parent.defaultRowHeightWeight : 100

    implicitHeight: Math.round(heightWeight)

    // HACK: Layout.preferredWidth is used as a sort of size hint, not an exact size.
    // If the weight is too large, preferredWidth gets capped out and stuff starts breaking.
    Layout.preferredWidth: weight / Math.max(defaultKeyWeight, 1)
    Layout.minimumWidth: 0
    Layout.preferredHeight: heightWeight
    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.verticalStretchFactor: Layout.fillHeight ? Math.round(heightWeight) : -1
    spacing: 0
}
