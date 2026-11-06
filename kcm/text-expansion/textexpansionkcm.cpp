/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "textexpansionkcm.h"

#include <qqml.h>

K_PLUGIN_CLASS_WITH_JSON(TextExpansionKcm, "kcm_textexpansion.json")

TextExpansionKcm::TextExpansionKcm(QObject *parent, const KPluginMetaData &metaData)
    : KQuickManagedConfigModule(parent, metaData)
    , m_settings(new PlasmaKeyboardSettings(this))
    , m_rulesModel(new TextExpansionModel(m_settings, this))
{
    qmlRegisterAnonymousType<PlasmaKeyboardSettings>("org.kde.plasma.keyboard.settings", 0);
}

PlasmaKeyboardSettings *TextExpansionKcm::settings() const
{
    return m_settings;
}

TextExpansionModel *TextExpansionKcm::rulesModel() const
{
    return m_rulesModel;
}

#include "textexpansionkcm.moc"

#include "moc_textexpansionkcm.cpp"
