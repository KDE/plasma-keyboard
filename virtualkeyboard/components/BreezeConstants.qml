// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

import org.kde.kirigami as Kirigami

pragma Singleton

QtObject {
    // Filled in by the style
    property real scaleHint

    readonly property string fontFamily: Kirigami.Theme.defaultFont.family
    readonly property real keyBackgroundHorizontalMargin: Math.round(8 * scaleHint)
    readonly property real keyBackgroundVerticalMargin: Math.round(12 * scaleHint)
    readonly property real keyContentMargin: Math.round(40 * scaleHint)
    readonly property real keyIconScale: scaleHint * 0.55

    readonly property bool isDark: Kirigami.ColorUtils.brightnessForColor(Kirigami.Theme.backgroundColor) === Kirigami.ColorUtils.Dark

    property color primaryColor: Kirigami.Theme.backgroundColor
    property color textOnPrimaryColor: Kirigami.Theme.textColor
    property color secondaryColor: Kirigami.Theme.backgroundColor
    property color textOnSecondaryColor: Kirigami.Theme.textColor

    property color keyboardBackgroundColor: primaryColor
    property color keyboardTopSeparatorColor: Qt.darker(primaryColor, 1.3)
    property color normalKeyBackgroundColor: Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, "white", isDark ? 0.1 : 0.75)
    property color normalKeyPressedBackgroundColor: isDark ? secondaryKeyBackgroundColor : Qt.darker(normalKeyBackgroundColor, 1.3)
    property color secondaryKeyBackgroundColor: Kirigami.ColorUtils.tintWithAlpha(primaryColor, normalKeyBackgroundColor, isDark ? 0.4 : (0.6 - Math.abs(primaryColor.hslLightness - normalKeyBackgroundColor.hslLightness)) * -1)
    property color secondaryKeyPressedBackgroundColor: Qt.darker(secondaryKeyBackgroundColor, 1.2)
    property color highlightedKeyBackgroundColor: Kirigami.ColorUtils.tintWithAlpha(normalKeyBackgroundColor, "white", 0.2)
    property color keyTextColor: textOnPrimaryColor
    property color keySmallTextColor: textOnPrimaryColor
    property color keyShadowColor: Qt.rgba(0, 0, 0, isDark ? 0.2 : 0.25)
    property real keyShadowSize: 3
    property real keyShadowYOffset: 1
    property color popupBackgroundColor: secondaryColor
    property color popupBorderColor: Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.textColor, secondaryColor, 0.9)
    property color popupTextColor: textOnSecondaryColor
    property color popupTextSelectedColor: textOnSecondaryColor
    property color popupHighlightBorderColor: Kirigami.Theme.highlightColor
    property color popupHighlightColor: Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.3)
    property color selectionListTextColor: textOnPrimaryColor
    property color selectionListSeparatorColor: Qt.lighter(primaryColor, 1.3)
    property color selectionListBackgroundColor: primaryColor
    property color navigationHighlightColor: Qt.rgba(navigationHighlightBorderColor.r, navigationHighlightBorderColor.g, navigationHighlightBorderColor.b, 0.3)
    property color navigationHighlightBorderColor: Kirigami.Theme.highlightColor

    readonly property real buttonRadius: Kirigami.Units.cornerRadius
    readonly property real popupRadius: Kirigami.Units.cornerRadius
}
