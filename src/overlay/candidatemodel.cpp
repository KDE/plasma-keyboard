/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "candidatemodel.h"

CandidateModel::CandidateModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int CandidateModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_candidates.size());
}

QVariant CandidateModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_candidates.size()) {
        return {};
    }

    const auto &candidate = m_candidates.at(index.row());

    switch (role) {
    case DisplayRole:
    case Qt::DisplayRole:
        return candidate.display;
    case InsertTextRole:
        return candidate.insertText.isEmpty() ? candidate.display : candidate.insertText;
    case DescriptionRole:
        return candidate.description;
    case CategoryRole:
        return candidate.category;
    case KeywordsRole:
        return candidate.keywords;
    default:
        return {};
    }
}

QHash<int, QByteArray> CandidateModel::roleNames() const
{
    return {
        {DisplayRole, "display"},
        {InsertTextRole, "insertText"},
        {DescriptionRole, "description"},
        {CategoryRole, "category"},
        {KeywordsRole, "keywords"},
    };
}

QString CandidateModel::query() const
{
    return m_query;
}

void CandidateModel::setQuery(const QString &query)
{
    if (m_query == query) {
        return;
    }
    m_query = query;
    Q_EMIT queryChanged();
}

QString CandidateModel::triggerId() const
{
    return m_triggerId;
}

void CandidateModel::setTriggerId(const QString &triggerId)
{
    if (m_triggerId == triggerId) {
        return;
    }
    m_triggerId = triggerId;
    Q_EMIT triggerIdChanged();
}

void CandidateModel::setCandidates(const QStringList &candidates)
{
    beginResetModel();
    m_candidates.clear();
    m_candidates.reserve(candidates.size());
    for (const auto &text : candidates) {
        m_candidates.append({text, text, {}, {}, {}});
    }
    endResetModel();
}

void CandidateModel::clear()
{
    beginResetModel();
    m_candidates.clear();
    endResetModel();
}

QString CandidateModel::insertTextAt(int index) const
{
    if (index < 0 || index >= m_candidates.size()) {
        return {};
    }
    const auto &candidate = m_candidates.at(index);
    return candidate.insertText.isEmpty() ? candidate.display : candidate.insertText;
}

#include "moc_candidatemodel.cpp"
