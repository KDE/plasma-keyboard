// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

import org.kde.kirigami as Kirigami

pragma Singleton

QtObject {
    // Filled in by the style
    property real scaleHint

    readonly property string fontFamily: Kirigami.Theme.defaultFont.family
    readonly property real keyBackgroundMargin: Math.round(8 * scaleHint)
    readonly property real keyContentMargin: Math.round(40 * scaleHint)
    readonly property real keyIconScale: scaleHint * 0.8

    property color primaryColor: Kirigami.Theme.backgroundColor
    property color primaryLightColor: Qt.lighter(primaryColor, 1.3)
    property color primaryDarkColor: Qt.darker(primaryColor, 1.3)
    property color textOnPrimaryColor: Kirigami.Theme.textColor
    property color secondaryColor: Kirigami.Theme.backgroundColor
    property color secondaryLightColor: Qt.lighter(secondaryColor, 1.3)
    property color secondaryDarkColor: Qt.darker(secondaryColor, 1.3)
    property color textOnSecondaryColor: Kirigami.Theme.textColor

    property color keyboardBackgroundColor: primaryColor
    property color normalKeyBackgroundColor: primaryLightColor
    property color normalKeyPressedBackgroundColor: primaryDarkColor
    property color highlightedKeyBackgroundColor: primaryLightColor
    property color capsLockKeyAccentColor: secondaryColor
    property color modeKeyAccentColor: textOnPrimaryColor
    property color keyTextColor: textOnPrimaryColor
    property color keySmallTextColor: textOnPrimaryColor
    property color popupBackgroundColor: secondaryColor
    property color popupBorderColor: Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.textColor, secondaryColor, 0.9)
    property color popupTextColor: textOnSecondaryColor
    property color popupTextSelectedColor: textOnSecondaryColor
    property color popupHighlightBorderColor: Kirigami.Theme.highlightColor
    property color popupHighlightColor: Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.3)
    property color selectionListTextColor: textOnPrimaryColor
    property color selectionListSeparatorColor: primaryLightColor
    property color selectionListBackgroundColor: primaryColor
    property color navigationHighlightColor: Qt.rgba(navigationHighlightBorderColor.r, navigationHighlightBorderColor.g, navigationHighlightBorderColor.b, 0.3)
    property color navigationHighlightBorderColor: Kirigami.Theme.highlightColor

    readonly property real buttonRadius: Kirigami.Units.cornerRadius
    readonly property real popupRadius: Kirigami.Units.cornerRadius
}
