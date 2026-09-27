/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "emojimodel.h"

#include <KEmoji/Emoji>

EmojiModel::EmojiModel(QObject *parent)
    : KEmoji::Model(parent)
{
    connect(this, &QAbstractItemModel::modelReset, this, [this]() {
        if (m_historyFrozen) {
            takeHistorySnapshot();
        }
    });
}

bool EmojiModel::historyFrozen() const
{
    return m_historyFrozen;
}

void EmojiModel::setHistoryFrozen(bool historyFrozen)
{
    if (historyFrozen == m_historyFrozen) {
        return;
    }

    if (historyFrozen) {
        takeHistorySnapshot();
        m_historyFrozen = true;
    } else {
        // Unfreeze
        beginResetModel();
        m_historyFrozen = false;
        m_recentIndices.clear();
        m_timesUsed.clear();
        endResetModel();
    }
    Q_EMIT historyFrozenChanged();
}

QVariant EmojiModel::data(const QModelIndex &index, int role) const
{
    if (!m_historyFrozen || (role != RecentIndexRole && role != TimesUsedRole)) {
        return KEmoji::Model::data(index, role);
    }

    QString id = KEmoji::Model::data(index, EmojiRole).value<KEmoji::Emoji>().id();
    if (role == RecentIndexRole) {
        return m_recentIndices.value(id, -1);
    }
    return m_timesUsed.value(id, 0);
}

void EmojiModel::takeHistorySnapshot()
{
    m_recentIndices.clear();
    m_timesUsed.clear();

    for (int row = 0; row < rowCount(); ++row) {
        QModelIndex idx = index(row);
        QString id = KEmoji::Model::data(idx, EmojiRole).value<KEmoji::Emoji>().id();
        if (int recentIndex = KEmoji::Model::data(idx, RecentIndexRole).toInt(); recentIndex >= 0) {
            m_recentIndices.insert(id, recentIndex);
        }
        if (int timesUsed = KEmoji::Model::data(idx, TimesUsedRole).toInt(); timesUsed > 0) {
            m_timesUsed.insert(id, timesUsed);
        }
    }
}

#include "moc_emojimodel.cpp"
