// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

Kirigami.ShadowedRectangle {
    property BreezeConstants theme

    // Use stronger shadow for dark theme for contrast
    shadow.size: Kirigami.ColorUtils.brightnessForColor(Kirigami.Theme.backgroundColor) === Kirigami.ColorUtils.Dark ? 20 : 5
    shadow.color: Qt.rgba(0, 0, 0, 0.2)

    color: theme.popupBackgroundColor
    radius: theme.popupRadius
    border {
        width: 1
        color: theme.popupBorderColor
    }
}
