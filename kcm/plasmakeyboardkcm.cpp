/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "plasmakeyboardkcm.h"
#include "../src/layoutpathhelper.h"

#include <qqml.h>

K_PLUGIN_CLASS_WITH_JSON(PlasmaKeyboardKcm, "kcm_plasmakeyboard.json")

PlasmaKeyboardKcm::PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData)
    : KQuickManagedConfigModule(parent, metaData)
    , m_settings(new PlasmaKeyboardSettings(this))
{
    initLayoutsPath();

    qmlRegisterAnonymousType<PlasmaKeyboardSettings>("org.kde.plasma.keyboard.settings", 0);
}

void PlasmaKeyboardKcm::enableLocale(const QString &locale)
{
    auto enabledLocales = plasmaKeyboardSettings()->enabledLocales();

    if (enabledLocales.contains(locale)) {
        return;
    }
    enabledLocales.append(locale);

    plasmaKeyboardSettings()->setEnabledLocales(enabledLocales);
}

void PlasmaKeyboardKcm::disableLocale(const QString &locale)
{
    auto enabledLocales = plasmaKeyboardSettings()->enabledLocales();

    if (!enabledLocales.contains(locale)) {
        return;
    }
    enabledLocales.removeAll(locale);

    plasmaKeyboardSettings()->setEnabledLocales(enabledLocales);
}

PlasmaKeyboardSettings *PlasmaKeyboardKcm::plasmaKeyboardSettings() const
{
    return m_settings;
}

#include "plasmakeyboardkcm.moc"

#include "moc_plasmakeyboardkcm.cpp"
