/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "plasmakeyboardkcm.h"

#include "keyboardlayoutmetadata.h"

#include <KRuntimePlatform>

#include <qqml.h>

K_PLUGIN_CLASS_WITH_JSON(PlasmaKeyboardKcm, "kcm_plasmakeyboard.json")

PlasmaKeyboardKcm::PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData)
    : KQuickManagedConfigModule(parent, metaData)
    , m_settings(new PlasmaKeyboardSettings(this))
    , m_availableKeyboardLayoutGroups(new KeyboardLayoutGroupModel(this))
{
    qmlRegisterAnonymousType<PlasmaKeyboardSettings>("org.kde.plasma.keyboard.settings", 0);

    m_keyboardLayoutFormFactorFilter = KRuntimePlatform::runtimePlatform();
    m_keyboardLayoutFormFactorFilter.removeAll(QString());
    if (m_keyboardLayoutFormFactorFilter.isEmpty()) {
        m_keyboardLayoutFormFactorFilter = {QStringLiteral("desktop")};
    }

    refreshAvailableKeyboardLayoutFilter();
    m_hasHiddenKeyboardLayouts = m_availableKeyboardLayoutGroups->layouts()->rowCount() < KeyboardLayoutMetadata::keyboardLayouts().size();

    QStringList enabledKeyboardLayoutIds = m_settings->enabledKeyboardLayoutIds();
    if (enabledKeyboardLayoutIds.isEmpty()) {
        const QStringList enabledLocales = m_settings->enabledLocales();
        for (const QString &locale : enabledLocales) {
            const QString layoutId = m_availableKeyboardLayoutGroups->layoutForLocale(locale);
            if (!layoutId.isEmpty() && !enabledKeyboardLayoutIds.contains(layoutId)) {
                enabledKeyboardLayoutIds.append(layoutId);
            }
        }
        if (!enabledKeyboardLayoutIds.isEmpty()) {
            m_settings->setEnabledKeyboardLayoutIds(enabledKeyboardLayoutIds);
            m_settings->setEnabledLocales({});
        }
    }
    m_availableKeyboardLayoutGroups->setEnabledLayoutIds(enabledKeyboardLayoutIds);

    connect(m_settings, &PlasmaKeyboardSettings::enabledKeyboardLayoutIdsChanged, this, [this]() {
        m_availableKeyboardLayoutGroups->setEnabledLayoutIds(m_settings->enabledKeyboardLayoutIds());
    });
}

PlasmaKeyboardSettings *PlasmaKeyboardKcm::plasmaKeyboardSettings() const
{
    return m_settings;
}

QAbstractItemModel *PlasmaKeyboardKcm::availableKeyboardLayouts() const
{
    return m_availableKeyboardLayoutGroups->layouts();
}

QAbstractItemModel *PlasmaKeyboardKcm::availableKeyboardLayoutGroups() const
{
    return m_availableKeyboardLayoutGroups;
}

bool PlasmaKeyboardKcm::hasHiddenKeyboardLayouts() const
{
    return m_hasHiddenKeyboardLayouts;
}

bool PlasmaKeyboardKcm::keyboardLayoutFormFactorFilterEnabled() const
{
    return m_keyboardLayoutFormFactorFilterEnabled;
}

void PlasmaKeyboardKcm::setKeyboardLayoutFormFactorFilterEnabled(bool enabled)
{
    if (enabled == m_keyboardLayoutFormFactorFilterEnabled) {
        return;
    }

    m_keyboardLayoutFormFactorFilterEnabled = enabled;
    Q_EMIT keyboardLayoutFormFactorFilterEnabledChanged();
    refreshAvailableKeyboardLayoutFilter();
}

void PlasmaKeyboardKcm::enableKeyboardLayout(const QString &layoutId)
{
    QStringList enabledKeyboardLayoutIds = m_settings->enabledKeyboardLayoutIds();
    if (enabledKeyboardLayoutIds.contains(layoutId)) {
        return;
    }

    enabledKeyboardLayoutIds.append(layoutId);
    m_settings->setEnabledKeyboardLayoutIds(enabledKeyboardLayoutIds);
}

void PlasmaKeyboardKcm::disableKeyboardLayout(const QString &layoutId)
{
    QStringList enabledKeyboardLayoutIds = m_settings->enabledKeyboardLayoutIds();
    if (!enabledKeyboardLayoutIds.removeAll(layoutId)) {
        return;
    }

    m_settings->setEnabledKeyboardLayoutIds(enabledKeyboardLayoutIds);
}

void PlasmaKeyboardKcm::refreshAvailableKeyboardLayoutFilter()
{
    m_availableKeyboardLayoutGroups->setFormFactorFilter(keyboardLayoutFormFactorFilterEnabled() ? m_keyboardLayoutFormFactorFilter : QStringList{});
}

#include "plasmakeyboardkcm.moc"

#include "moc_plasmakeyboardkcm.cpp"
