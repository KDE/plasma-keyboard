/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <KQuickManagedConfigModule>

#include "plasmakeyboardsettings.h"

class PlasmaKeyboardKcm : public KQuickManagedConfigModule
{
    Q_OBJECT
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)
    Q_PROPERTY(bool vibrationEnabled READ vibrationEnabled WRITE setVibrationEnabled NOTIFY vibrationEnabledChanged)
    Q_PROPERTY(bool keyboardNavigationEnabled READ keyboardNavigationEnabled WRITE setKeyboardNavigationEnabled NOTIFY keyboardNavigationEnabledChanged)

public:
    PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData);

    bool soundEnabled() const;
    void setSoundEnabled(bool soundEnabled);

    bool vibrationEnabled() const;
    void setVibrationEnabled(bool vibrationEnabled);

    bool keyboardNavigationEnabled() const;
    void setKeyboardNavigationEnabled(bool keyboardNavigationEnabled);

    bool isSaveNeeded() const override;

public Q_SLOTS:
    void load() override;
    void save() override;

Q_SIGNALS:
    void soundEnabledChanged();
    void vibrationEnabledChanged();
    void keyboardNavigationEnabledChanged();

private:
    bool m_soundEnabled = false;
    bool m_vibrationEnabled = true;
    bool m_keyboardNavigationEnabled = false;

    bool m_saveNeeded = false;

    PlasmaKeyboardSettings *m_settings = nullptr;
};
