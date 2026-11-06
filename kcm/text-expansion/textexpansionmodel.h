/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QAbstractListModel>

class PlasmaKeyboardSettings;

class TextExpansionModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Role {
        AbbreviationRole = Qt::UserRole + 1,
        ExpansionRole,
    };
    Q_ENUM(Role)

    explicit TextExpansionModel(PlasmaKeyboardSettings *settings, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    int count() const;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE bool addRule(const QString &abbreviation, const QString &expansion);
    Q_INVOKABLE bool updateRule(int row, const QString &abbreviation, const QString &expansion);
    Q_INVOKABLE void removeRule(int row);
    Q_INVOKABLE QVariantMap ruleAt(int row) const;

Q_SIGNALS:
    void countChanged();

private:
    struct Rule {
        QString abbreviation;
        QString expansion;
    };

    void loadRules();
    void saveRules();
    bool abbreviationExists(const QString &abbreviation, int exceptRow = -1) const;

    PlasmaKeyboardSettings *m_settings;
    QList<Rule> m_rules;
};
