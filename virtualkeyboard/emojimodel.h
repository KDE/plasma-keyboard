/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QHash>
#include <QString>
#include <qqmlintegration.h>

#include <KEmoji/Model>

/**
 * A KEmoji::Model that can freeze the emoji usage history it reports.
 */
class EmojiModel : public KEmoji::Model
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool historyFrozen READ historyFrozen WRITE setHistoryFrozen NOTIFY historyFrozenChanged)

public:
    explicit EmojiModel(QObject *parent = nullptr);

    bool historyFrozen() const;
    void setHistoryFrozen(bool historyFrozen);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void historyFrozenChanged();

private:
    void takeHistorySnapshot();

    bool m_historyFrozen = false;
    QHash<QString, int> m_recentIndices;
    QHash<QString, int> m_timesUsed;
};
