/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "plasmakeyboardkcm.h"

#include <KRuntimePlatform>

K_PLUGIN_CLASS_WITH_JSON(PlasmaKeyboardKcm, "kcm_plasmakeyboard.json")

PlasmaKeyboardKcm::PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData)
    : KQuickManagedConfigModule(parent, metaData)
    , m_availableKeyboardLayoutGroups(new KeyboardLayoutGroupModel(this))
{
    // clang-format off
    qmlRegisterSingletonInstance<PlasmaKeyboardSettings>(
        "org.kde.plasma.keyboard.settings",
        1,
        0,
        "PlasmaKeyboardSettings",
        PlasmaKeyboardSettings::self()
    );
    // clang-format on

    refreshAvailableKeyboardLayoutFilter();
    load();
}

bool PlasmaKeyboardKcm::soundEnabled() const
{
    return m_soundEnabled;
}

void PlasmaKeyboardKcm::setSoundEnabled(bool soundEnabled)
{
    if (soundEnabled == m_soundEnabled) {
        return;
    }

    m_soundEnabled = soundEnabled;
    Q_EMIT soundEnabledChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::vibrationEnabled() const
{
    return m_vibrationEnabled;
}

void PlasmaKeyboardKcm::setVibrationEnabled(bool vibrationEnabled)
{
    if (vibrationEnabled == m_vibrationEnabled) {
        return;
    }

    m_vibrationEnabled = vibrationEnabled;
    Q_EMIT vibrationEnabledChanged();

    setNeedsSave(true);
}

QStringList PlasmaKeyboardKcm::enabledKeyboardLayoutIds() const
{
    return m_enabledKeyboardLayoutIds;
}

QAbstractItemModel *PlasmaKeyboardKcm::availableKeyboardLayouts() const
{
    return m_availableKeyboardLayoutGroups->layouts();
}

QAbstractItemModel *PlasmaKeyboardKcm::availableKeyboardLayoutGroups() const
{
    return m_availableKeyboardLayoutGroups;
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
    if (m_enabledKeyboardLayoutIds.contains(layoutId)) {
        return;
    }

    m_enabledKeyboardLayoutIds.append(layoutId);
    m_availableKeyboardLayoutGroups->setEnabledLayoutIds(m_enabledKeyboardLayoutIds);
    Q_EMIT enabledKeyboardLayoutIdsChanged();

    setNeedsSave(true);
}

void PlasmaKeyboardKcm::disableKeyboardLayout(const QString &layoutId)
{
    if (!m_enabledKeyboardLayoutIds.contains(layoutId)) {
        return;
    }

    m_enabledKeyboardLayoutIds.removeAll(layoutId);
    m_availableKeyboardLayoutGroups->setEnabledLayoutIds(m_enabledKeyboardLayoutIds);
    Q_EMIT enabledKeyboardLayoutIdsChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::keyboardNavigationEnabled() const
{
    return m_keyboardNavigationEnabled;
}

void PlasmaKeyboardKcm::setKeyboardNavigationEnabled(bool keyboardNavigationEnabled)
{
    if (keyboardNavigationEnabled == m_keyboardNavigationEnabled) {
        return;
    }

    m_keyboardNavigationEnabled = keyboardNavigationEnabled;
    Q_EMIT keyboardNavigationEnabledChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::diacriticsPopupEnabled() const
{
    return m_diacriticsPopupEnabled;
}

void PlasmaKeyboardKcm::setDiacriticsPopupEnabled(bool enabled)
{
    if (enabled == m_diacriticsPopupEnabled) {
        return;
    }

    m_diacriticsPopupEnabled = enabled;
    Q_EMIT diacriticsPopupEnabledChanged();

    setNeedsSave(true);
}

int PlasmaKeyboardKcm::diacriticsHoldThresholdMs() const
{
    return m_diacriticsHoldThresholdMs;
}

void PlasmaKeyboardKcm::setDiacriticsHoldThresholdMs(int thresholdMs)
{
    if (thresholdMs == m_diacriticsHoldThresholdMs) {
        return;
    }

    m_diacriticsHoldThresholdMs = thresholdMs;
    Q_EMIT diacriticsHoldThresholdMsChanged();

    setNeedsSave(true);
}

void PlasmaKeyboardKcm::load()
{
    setSoundEnabled(PlasmaKeyboardSettings::self()->soundEnabled());
    setVibrationEnabled(PlasmaKeyboardSettings::self()->vibrationEnabled());

    m_enabledKeyboardLayoutIds = PlasmaKeyboardSettings::self()->enabledKeyboardLayoutIds();
    if (m_enabledKeyboardLayoutIds.isEmpty()) {
        KeyboardLayoutGroupModel availableKeyboardLayouts;
        availableKeyboardLayouts.loadInstalledGroups();
        const QStringList enabledLocales = PlasmaKeyboardSettings::self()->enabledLocales();
        for (const auto &locale : enabledLocales) {
            const QString layoutId = availableKeyboardLayouts.layoutForLocale(locale);
            if (!layoutId.isEmpty() && !m_enabledKeyboardLayoutIds.contains(layoutId)) {
                m_enabledKeyboardLayoutIds.append(layoutId);
            }
        }
    }
    m_availableKeyboardLayoutGroups->setEnabledLayoutIds(m_enabledKeyboardLayoutIds);
    Q_EMIT enabledKeyboardLayoutIdsChanged();
    setKeyboardNavigationEnabled(PlasmaKeyboardSettings::self()->keyboardNavigationEnabled());
    setDiacriticsPopupEnabled(PlasmaKeyboardSettings::self()->diacriticsPopupEnabled());
    setDiacriticsHoldThresholdMs(PlasmaKeyboardSettings::self()->diacriticsHoldThresholdMs());

    setNeedsSave(false);
}

void PlasmaKeyboardKcm::save()
{
    PlasmaKeyboardSettings::self()->setSoundEnabled(m_soundEnabled);
    PlasmaKeyboardSettings::self()->setVibrationEnabled(m_vibrationEnabled);
    PlasmaKeyboardSettings::self()->setEnabledKeyboardLayoutIds(m_enabledKeyboardLayoutIds);
    PlasmaKeyboardSettings::self()->setEnabledLocales({});
    PlasmaKeyboardSettings::self()->setKeyboardNavigationEnabled(m_keyboardNavigationEnabled);
    PlasmaKeyboardSettings::self()->setDiacriticsPopupEnabled(m_diacriticsPopupEnabled);
    PlasmaKeyboardSettings::self()->setDiacriticsHoldThresholdMs(m_diacriticsHoldThresholdMs);
    PlasmaKeyboardSettings::self()->save();

    setNeedsSave(false);
}

void PlasmaKeyboardKcm::refreshAvailableKeyboardLayoutFilter()
{
    QStringList formFactorFilter;
    if (keyboardLayoutFormFactorFilterEnabled()) {
        formFactorFilter = KRuntimePlatform::runtimePlatform();
        formFactorFilter.removeAll(QString());
        if (formFactorFilter.isEmpty()) {
            formFactorFilter = {QStringLiteral("desktop")};
        }
    }
    m_availableKeyboardLayoutGroups->setFormFactorFilter(formFactorFilter);
}

#include "plasmakeyboardkcm.moc"

#include "moc_plasmakeyboardkcm.cpp"
