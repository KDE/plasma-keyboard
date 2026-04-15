/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "keyboardlayoutmodel.h"

#include <QAbstractListModel>
#include <QStringList>

class KeyboardLayoutGroupModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)

public:
    enum Role {
        GroupIdRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        LayoutsRole,
        VisibleRole,
        DisplayExpandedRole,
        SelectedCountRole,
        VisibleLayoutCountRole,
    };
    Q_ENUM(Role)

    struct Group {
        QString groupId;
        QString name;
        QString description;
        KeyboardLayoutModel *layouts = nullptr;
    };

    explicit KeyboardLayoutGroupModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    KeyboardLayoutModel *layouts() const;
    QString layoutForLocale(const QString &locale) const;
    QString filterText() const;
    void setFilterText(const QString &filterText);
    void setEnabledLayoutIds(const QStringList &enabledLayoutIds);
    Q_INVOKABLE void toggleExpanded(const QString &groupId);

    void loadInstalledGroups();
    void setFormFactorFilter(const QStringList &formFactorFilter);

Q_SIGNALS:
    void filterTextChanged();

private:
    bool matchesFormFactor(const QStringList &formFactors) const;
    bool groupNameMatches(const Group &group) const;
    void clearGroups();
    void refreshChildModels();
    void emitDataChanged();

    QList<Group> m_groups;
    KeyboardLayoutModel *m_flatLayouts = nullptr;
    QStringList m_formFactorFilter;
    QString m_filterText;
    QStringList m_enabledLayoutIds;
    QStringList m_expandedGroupIds;
};
