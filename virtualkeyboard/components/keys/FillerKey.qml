// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts

// Invisible spacer to align keyboard rows without creating a key.

Item {
    property real weight: parent && parent.defaultKeyWeight !== undefined ? parent.defaultKeyWeight : 100

    // HACK: Layout.preferredWidth is used as a sort of size hint, not an exact size.
    // If the weight is too large, preferredWidth gets capped out and stuff starts breaking.
    Layout.preferredWidth: weight / Math.max(parent && parent.defaultKeyWeight !== undefined ? parent.defaultKeyWeight : 100, 1)
    Layout.minimumWidth: 0
    Layout.fillWidth: true
    Layout.fillHeight: true
}
