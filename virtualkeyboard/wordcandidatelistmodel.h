/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QAbstractListModel>

class InputEngine;

class WordCandidateListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Role {
        DisplayRole = Qt::UserRole + 1,
    };
    Q_ENUM(Role)

    explicit WordCandidateListModel(InputEngine *inputEngine, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;

    Q_INVOKABLE void selectItem(int index);

Q_SIGNALS:
    void countChanged();

private:
    void refresh();

    InputEngine *m_inputEngine = nullptr;
    QStringList m_candidates;
};
