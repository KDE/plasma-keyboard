/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QStringList>
#include <QVariantMap>

class KeyboardLayoutMetadata
{
public:
    struct Layout {
        QString id;
        QString packageId;
        QString packagePath;
        QString layoutId;
        QString packageName;
        QStringList locales;
        QStringList formFactors;
        QVariantMap definition;
    };

    static QString localeDisplayName(const QString &locale);
    static QString keyboardLayoutName(const QVariantMap &layout);
    static QString keyboardLayoutFile(const QVariantMap &layout, const QString &layoutType);
    static QStringList keyboardLayoutFormFactors(const QVariantMap &layout);
    static QString keyboardPackageDisplayName(const Layout &layout);
    static QList<Layout> keyboardLayouts();
};
