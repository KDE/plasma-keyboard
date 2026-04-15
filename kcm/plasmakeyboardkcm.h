/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <KQuickManagedConfigModule>

#include "keyboardlayoutgroupmodel.h"
#include "plasmakeyboardsettings.h"

class QAbstractItemModel;

class PlasmaKeyboardKcm : public KQuickManagedConfigModule
{
    Q_OBJECT
    Q_PROPERTY(PlasmaKeyboardSettings *plasmaKeyboardSettings READ plasmaKeyboardSettings CONSTANT)
    Q_PROPERTY(QAbstractItemModel *availableKeyboardLayouts READ availableKeyboardLayouts CONSTANT)
    Q_PROPERTY(QAbstractItemModel *availableKeyboardLayoutGroups READ availableKeyboardLayoutGroups CONSTANT)
    Q_PROPERTY(bool hasHiddenKeyboardLayouts READ hasHiddenKeyboardLayouts CONSTANT)
    Q_PROPERTY(bool keyboardLayoutFormFactorFilterEnabled READ keyboardLayoutFormFactorFilterEnabled WRITE setKeyboardLayoutFormFactorFilterEnabled NOTIFY
                   keyboardLayoutFormFactorFilterEnabledChanged)

public:
    PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData);

    PlasmaKeyboardSettings *plasmaKeyboardSettings() const;
    QAbstractItemModel *availableKeyboardLayouts() const;
    QAbstractItemModel *availableKeyboardLayoutGroups() const;
    bool hasHiddenKeyboardLayouts() const;

    bool keyboardLayoutFormFactorFilterEnabled() const;
    void setKeyboardLayoutFormFactorFilterEnabled(bool enabled);

    Q_INVOKABLE void enableKeyboardLayout(const QString &layoutId);
    Q_INVOKABLE void disableKeyboardLayout(const QString &layoutId);

Q_SIGNALS:
    void keyboardLayoutFormFactorFilterEnabledChanged();

private:
    void refreshAvailableKeyboardLayoutFilter();

    PlasmaKeyboardSettings *m_settings = nullptr;
    KeyboardLayoutGroupModel *m_availableKeyboardLayoutGroups = nullptr;
    QStringList m_keyboardLayoutFormFactorFilter;
    bool m_hasHiddenKeyboardLayouts = false;
    bool m_keyboardLayoutFormFactorFilterEnabled = true;
};
