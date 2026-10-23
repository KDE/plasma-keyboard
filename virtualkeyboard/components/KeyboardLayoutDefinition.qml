/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard

/**
 * Defines a keyboard layout and its alternative layouts.
 *
 * This is what "File" in keyboard package metadata points to for a keyboard layout.
 */
QtObject {
    /**
     * The text composer used by layouts.
     * This can be overridden, see KeyboardLayout.textComposer.
     */
    property QtObject textComposer: BasicTextComposer {}

    /**
     * The default layout.
     */
    default property Component main

    /**
     * The layout shown in symbol mode.
     */
    property Component symbols

    /**
     * The layout for fields that take formatted numbers (Qt::ImhFormattedNumbersOnly).
     */
    property Component numbers

    /**
     * The layout for fields that only take digits (Qt::ImhDigitsOnly).
     */
    property Component digits

    /**
     * The layout for fields that take phone numbers (Qt::ImhDialableCharactersOnly).
     */
    property Component dialpad
}
