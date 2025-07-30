/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "plasmakeyboardkcm.h"

K_PLUGIN_CLASS_WITH_JSON(PlasmaKeyboardKcm, "kcm_plasmakeyboard.json")

PlasmaKeyboardKcm::PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData)
    : KQuickManagedConfigModule(parent, metaData)
{
    qmlRegisterSingletonInstance<PlasmaKeyboardSettings>("org.kde.plasma.keyboard.settings", 1, 0,
        "PlasmaKeyboardSettings", PlasmaKeyboardSettings::self());

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

bool PlasmaKeyboardKcm::isSaveNeeded() const
{
    return m_saveNeeded;
}

void PlasmaKeyboardKcm::load()
{
    setSoundEnabled(PlasmaKeyboardSettings::self()->soundEnabled());
    setVibrationEnabled(PlasmaKeyboardSettings::self()->vibrationEnabled());
    setKeyboardNavigationEnabled(PlasmaKeyboardSettings::self()->keyboardNavigationEnabled());

    setNeedsSave(false);
}

void PlasmaKeyboardKcm::save()
{
    PlasmaKeyboardSettings::self()->setSoundEnabled(m_soundEnabled);
    PlasmaKeyboardSettings::self()->setVibrationEnabled(m_vibrationEnabled);
    PlasmaKeyboardSettings::self()->setKeyboardNavigationEnabled(m_keyboardNavigationEnabled);
    PlasmaKeyboardSettings::self()->save();

    setNeedsSave(false);
}

#include "plasmakeyboardkcm.moc"
