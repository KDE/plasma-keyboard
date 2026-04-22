/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <KPluginMetaData>
#include <KQuickManagedConfigModule>

#include "keyboardlayoutgroupmodel.h"
#include "plasmakeyboardsettings.h"

class QAbstractItemModel;

class PlasmaKeyboardKcm : public KQuickManagedConfigModule
{
    Q_OBJECT
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)
    Q_PROPERTY(bool vibrationEnabled READ vibrationEnabled WRITE setVibrationEnabled NOTIFY vibrationEnabledChanged)
    Q_PROPERTY(QStringList enabledKeyboardLayoutIds READ enabledKeyboardLayoutIds NOTIFY enabledKeyboardLayoutIdsChanged)
    Q_PROPERTY(QAbstractItemModel *availableKeyboardLayouts READ availableKeyboardLayouts CONSTANT)
    Q_PROPERTY(QAbstractItemModel *availableKeyboardLayoutGroups READ availableKeyboardLayoutGroups CONSTANT)
    Q_PROPERTY(bool keyboardLayoutFormFactorFilterEnabled READ keyboardLayoutFormFactorFilterEnabled WRITE setKeyboardLayoutFormFactorFilterEnabled NOTIFY
                   keyboardLayoutFormFactorFilterEnabledChanged)
    Q_PROPERTY(bool keyboardNavigationEnabled READ keyboardNavigationEnabled WRITE setKeyboardNavigationEnabled NOTIFY keyboardNavigationEnabledChanged)
    Q_PROPERTY(bool diacriticsPopupEnabled READ diacriticsPopupEnabled WRITE setDiacriticsPopupEnabled NOTIFY diacriticsPopupEnabledChanged)
    Q_PROPERTY(int diacriticsHoldThresholdMs READ diacriticsHoldThresholdMs WRITE setDiacriticsHoldThresholdMs NOTIFY diacriticsHoldThresholdMsChanged)

public:
    PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData);

    bool soundEnabled() const;
    void setSoundEnabled(bool soundEnabled);

    bool vibrationEnabled() const;
    void setVibrationEnabled(bool vibrationEnabled);

    QStringList enabledKeyboardLayoutIds() const;
    QAbstractItemModel *availableKeyboardLayouts() const;
    QAbstractItemModel *availableKeyboardLayoutGroups() const;
    bool keyboardLayoutFormFactorFilterEnabled() const;
    void setKeyboardLayoutFormFactorFilterEnabled(bool enabled);

    Q_INVOKABLE void enableKeyboardLayout(const QString &layoutId);
    Q_INVOKABLE void disableKeyboardLayout(const QString &layoutId);

    bool keyboardNavigationEnabled() const;
    void setKeyboardNavigationEnabled(bool keyboardNavigationEnabled);

    bool diacriticsPopupEnabled() const;
    void setDiacriticsPopupEnabled(bool enabled);

    int diacriticsHoldThresholdMs() const;
    void setDiacriticsHoldThresholdMs(int thresholdMs);

public Q_SLOTS:
    void load() override;
    void save() override;

Q_SIGNALS:
    void soundEnabledChanged();
    void vibrationEnabledChanged();
    void enabledKeyboardLayoutIdsChanged();
    void keyboardLayoutFormFactorFilterEnabledChanged();
    void keyboardNavigationEnabledChanged();
    void diacriticsPopupEnabledChanged();
    void diacriticsHoldThresholdMsChanged();

private:
    void refreshAvailableKeyboardLayoutFilter();

    bool m_soundEnabled = false;
    bool m_vibrationEnabled = true;
    bool m_keyboardLayoutFormFactorFilterEnabled = true;
    bool m_keyboardNavigationEnabled = false;
    bool m_diacriticsPopupEnabled = true;
    int m_diacriticsHoldThresholdMs = 100;

    QStringList m_enabledKeyboardLayoutIds;
    KeyboardLayoutGroupModel *m_availableKeyboardLayoutGroups = nullptr;
};
