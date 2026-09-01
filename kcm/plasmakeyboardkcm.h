/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <KQuickManagedConfigModule>

#include "plasmakeyboardsettings.h"

class PlasmaKeyboardKcm : public KQuickManagedConfigModule
{
    Q_OBJECT
    Q_PROPERTY(PlasmaKeyboardSettings *plasmaKeyboardSettings READ plasmaKeyboardSettings CONSTANT)

public:
    PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData);

    PlasmaKeyboardSettings *plasmaKeyboardSettings() const;

    Q_INVOKABLE void enableLocale(const QString &locale);
    Q_INVOKABLE void disableLocale(const QString &locale);

private:
    PlasmaKeyboardSettings *m_settings = nullptr;
};
