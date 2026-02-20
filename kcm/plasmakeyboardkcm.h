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
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)
    Q_PROPERTY(bool vibrationEnabled READ vibrationEnabled WRITE setVibrationEnabled NOTIFY vibrationEnabledChanged)
    Q_PROPERTY(QStringList enabledLocales READ enabledLocales NOTIFY enabledLocalesChanged)
    Q_PROPERTY(bool keyboardNavigationEnabled READ keyboardNavigationEnabled WRITE setKeyboardNavigationEnabled NOTIFY keyboardNavigationEnabledChanged)
    Q_PROPERTY(bool diacriticsPopupEnabled READ diacriticsPopupEnabled WRITE setDiacriticsPopupEnabled NOTIFY diacriticsPopupEnabledChanged)
    Q_PROPERTY(int diacriticsHoldThresholdMs READ diacriticsHoldThresholdMs WRITE setDiacriticsHoldThresholdMs NOTIFY diacriticsHoldThresholdMsChanged)

public:
    PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData);

    bool soundEnabled() const;
    void setSoundEnabled(bool soundEnabled);

    bool vibrationEnabled() const;
    void setVibrationEnabled(bool vibrationEnabled);

    QStringList enabledLocales() const;

    Q_INVOKABLE void enableLocale(const QString &locale);
    Q_INVOKABLE void disableLocale(const QString &locale);

    bool keyboardNavigationEnabled() const;
    void setKeyboardNavigationEnabled(bool keyboardNavigationEnabled);

    bool diacriticsPopupEnabled() const;
    void setDiacriticsPopupEnabled(bool enabled);

    int diacriticsHoldThresholdMs() const;
    void setDiacriticsHoldThresholdMs(int thresholdMs);

    bool isSaveNeeded() const override;

public Q_SLOTS:
    void load() override;
    void save() override;

Q_SIGNALS:
    void soundEnabledChanged();
    void vibrationEnabledChanged();
    void enabledLocalesChanged();
    void keyboardNavigationEnabledChanged();
    void diacriticsPopupEnabledChanged();
    void diacriticsHoldThresholdMsChanged();

private:
    bool m_soundEnabled = false;
    bool m_vibrationEnabled = true;
    bool m_keyboardNavigationEnabled = false;
    bool m_diacriticsPopupEnabled = true;
    int m_diacriticsHoldThresholdMs = 600;

    bool m_saveNeeded = false;

    QStringList m_enabledLocales;

    PlasmaKeyboardSettings *m_settings = nullptr;
};
