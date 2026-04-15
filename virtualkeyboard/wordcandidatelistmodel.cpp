/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "wordcandidatelistmodel.h"

#include "inputengine.h"

WordCandidateListModel::WordCandidateListModel(InputEngine *inputEngine, QObject *parent)
    : QAbstractListModel(parent)
    , m_inputEngine(inputEngine)
    , m_candidates(inputEngine ? inputEngine->candidates() : QStringList())
{
    Q_ASSERT(m_inputEngine);

    connect(m_inputEngine, &InputEngine::candidatesChanged, this, &WordCandidateListModel::refresh);
}

int WordCandidateListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_candidates.size();
}

QVariant WordCandidateListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_candidates.size()) {
        return {};
    }

    switch (role) {
    case DisplayRole:
    case Qt::DisplayRole:
        return m_candidates.at(index.row());
    default:
        return {};
    }
}

QHash<int, QByteArray> WordCandidateListModel::roleNames() const
{
    return {
        {DisplayRole, "display"},
    };
}

int WordCandidateListModel::count() const
{
    return m_candidates.size();
}

void WordCandidateListModel::selectItem(int index)
{
    if (!m_inputEngine || index < 0 || index >= m_candidates.size()) {
        return;
    }

    m_inputEngine->selectCandidate(index);
}

void WordCandidateListModel::refresh()
{
    const QStringList candidates = m_inputEngine ? m_inputEngine->candidates() : QStringList();
    if (m_candidates == candidates) {
        return;
    }

    const int oldCount = m_candidates.size();
    beginResetModel();
    m_candidates = candidates;
    endResetModel();
    if (oldCount != m_candidates.size()) {
        Q_EMIT countChanged();
    }
}
