/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "plasmakeyboardsettings.h"
#include "textexpansionmodel.h"

#include <KQuickManagedConfigModule>

class TextExpansionKcm : public KQuickManagedConfigModule
{
    Q_OBJECT
    Q_PROPERTY(PlasmaKeyboardSettings *settings READ settings CONSTANT)
    Q_PROPERTY(TextExpansionModel *rulesModel READ rulesModel CONSTANT)
    Q_PROPERTY(bool editorDirty READ editorDirty WRITE setEditorDirty NOTIFY editorDirtyChanged)

public:
    TextExpansionKcm(QObject *parent, const KPluginMetaData &metaData);

    PlasmaKeyboardSettings *settings() const;
    TextExpansionModel *rulesModel() const;
    bool editorDirty() const;
    void setEditorDirty(bool dirty);

public Q_SLOTS:
    void load() override;
    void save() override;
    void defaults() override;

Q_SIGNALS:
    void editorDirtyChanged();
    void applyEditor();

private:
    bool isSaveNeeded() const override;

    PlasmaKeyboardSettings *m_settings;
    TextExpansionModel *m_rulesModel;
    bool m_editorDirty = false;
};
