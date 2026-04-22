/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QAbstractListModel>
#include <QStringList>

class KeyboardLayoutModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role {
        LayoutIdRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        VisibleRole,
        EnabledRole,
    };
    Q_ENUM(Role)

    struct Layout {
        QString layoutId;
        QString name;
        QString description;
        QStringList locales;
    };

    explicit KeyboardLayoutModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setFilterText(const QString &filterText);
    void setEnabledLayoutIds(const QStringList &enabledLayoutIds);
    int visibleLayoutCount() const;
    int enabledVisibleLayoutCount() const;
    QString layoutForLocale(const QString &locale) const;
    void setLayouts(QList<Layout> layouts);
    static QString descriptionForLocales(const QStringList &locales);
    static bool matchesFilter(const Layout &layout, const QString &filterText);

private:
    void emitDataChanged();

    QList<Layout> m_layouts;
    QString m_filterText;
    QStringList m_enabledLayoutIds;
};
