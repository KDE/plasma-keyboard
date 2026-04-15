/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QStringList>
#include <QVariantMap>

/**
 * Discovers installed keyboard layouts and reads their package metadata.
 */
class KeyboardLayoutMetadata
{
public:
    struct Layout {
        QString packageId;
        QString layoutId;
        QString packagePath;
        QString keyboardName;
        QStringList locales;
        QStringList formFactors;
        QVariantMap layoutDefinition;
    };

    static QString localeDisplayName(const QString &locale);
    static QString keyboardLayoutName(const QVariantMap &layout);
    static QString keyboardLayoutFile(const QVariantMap &layout, const QString &layoutType);
    static QStringList keyboardLayoutFormFactors(const QVariantMap &layout);
    static QString keyboardDisplayName(const Layout &layout);
    static QList<Layout> keyboardLayouts();
};
