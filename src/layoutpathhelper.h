/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QDir>
#include <QStandardPaths>

/**
 * Initializes the QT_VIRTUALKEYBOARD_LAYOUT_PATH environment variable to point to our own keyboard layouts
 * provided in this repository, unless the PLASMA_KEYBOARD_USE_QT_LAYOUTS environment variable is set to "1" or "true".
 *
 * This allows us to provide our own custom layouts and override Qt's built-in ones, while still giving users
 * the option to use the default Qt layouts if they prefer.
 *
 * Helper function used by both the main application and the KCM, to ensure they both use the same layouts path.
 */
inline void initLayoutsPath()
{
    // PLASMA_KEYBOARD_USE_QT_LAYOUTS - whether to use Qt's builtin keyboard layouts rather than our own.
    bool useQtLayouts = QByteArrayList{"1", "true"}.contains(qgetenv("PLASMA_KEYBOARD_USE_QT_LAYOUTS").toLower());

    if (!useQtLayouts) {
        // Set QT_VIRTUALKEYBOARD_LAYOUT_PATH to our own keyboard layouts provided in this repository.

        // Loop over all "/usr/share" paths and check if layouts folder exists
        const QStringList locations = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
        for (const QString &basePath : locations) {
            QString layoutsDir = basePath + QStringLiteral("/plasma/keyboard/layouts");

            // Check if path exists
            if (QDir(layoutsDir).exists()) {
                // Set path for Qt to search for layouts
                qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", layoutsDir.toUtf8());
                break;
            }
        }
    }
}
