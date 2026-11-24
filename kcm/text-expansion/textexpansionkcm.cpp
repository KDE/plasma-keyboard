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
    connect(this, &KQuickConfigModule::pageRemoved, this, [this] {
        setEditorDirty(false);
    });
}

PlasmaKeyboardSettings *TextExpansionKcm::settings() const
{
    return m_settings;
}

TextExpansionModel *TextExpansionKcm::rulesModel() const
{
    return m_rulesModel;
}

bool TextExpansionKcm::editorDirty() const
{
    return m_editorDirty;
}

void TextExpansionKcm::setEditorDirty(bool dirty)
{
    if (m_editorDirty == dirty) {
        return;
    }
    m_editorDirty = dirty;
    Q_EMIT editorDirtyChanged();
    settingsChanged();
}

bool TextExpansionKcm::isSaveNeeded() const
{
    return m_editorDirty;
}

void TextExpansionKcm::load()
{
    if (depth() > 1) {
        pop();
    }
    KQuickManagedConfigModule::load();
    settingsChanged();
}

void TextExpansionKcm::save()
{
    if (m_editorDirty) {
        Q_EMIT applyEditor();
        if (m_editorDirty) {
            return;
        }
    }
    KQuickManagedConfigModule::save();
    settingsChanged();
}

void TextExpansionKcm::defaults()
{
    if (depth() > 1) {
        pop();
    }
    KQuickManagedConfigModule::defaults();
    settingsChanged();
}

#include "textexpansionkcm.moc"

#include "moc_textexpansionkcm.cpp"
