/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "keyboardlayoutmodel.h"

#include <QAbstractListModel>
#include <QHash>
#include <QStringList>

class EnabledKeyboardLayoutModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role {
        LayoutIdRole = Qt::UserRole + 1,
        NameRole,
    };
    Q_ENUM(Role)

    explicit EnabledKeyboardLayoutModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QStringList layoutIds() const;
    void setLayoutIds(const QStringList &layoutIds);
    bool move(int from, int to);

private:
    QHash<QString, KeyboardLayoutModel::Layout> m_installedLayouts;
    QStringList m_layoutIds;
};
