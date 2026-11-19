/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <KQuickManagedConfigModule>

#include "plasmakeyboardsettings.h"
#include "textexpansionmodel.h"

class TextExpansionKcm : public KQuickManagedConfigModule
{
    Q_OBJECT
    Q_PROPERTY(PlasmaKeyboardSettings *settings READ settings CONSTANT)
    Q_PROPERTY(TextExpansionModel *rulesModel READ rulesModel CONSTANT)

public:
    TextExpansionKcm(QObject *parent, const KPluginMetaData &metaData);

    PlasmaKeyboardSettings *settings() const;
    TextExpansionModel *rulesModel() const;

private:
    PlasmaKeyboardSettings *m_settings;
    TextExpansionModel *m_rulesModel;
};
