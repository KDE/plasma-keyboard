// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard

// Highlight drawn over an item focused by keyboard navigation.

Rectangle {
    color: BreezeConstants.navigationHighlightColor
    border.width: 3
    border.color: BreezeConstants.navigationHighlightBorderColor
    radius: BreezeConstants.buttonRadius
}
