/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "textexpansionmodel.h"

#include "plasmakeyboardsettings.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScopedValueRollback>

#include <algorithm>

TextExpansionModel::TextExpansionModel(PlasmaKeyboardSettings *settings, QObject *parent)
    : QAbstractListModel(parent)
    , m_settings(settings)
{
    loadRules();
    connect(m_settings, &PlasmaKeyboardSettings::textExpansionRulesChanged, this, [this] {
        if (!m_writingRules) {
            loadRules();
        }
    });
}

int TextExpansionModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_rules.size();
}

int TextExpansionModel::count() const
{
    return m_rules.size();
}

QVariant TextExpansionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_rules.size()) {
        return {};
    }

    const Rule &rule = m_rules.at(index.row());
    switch (role) {
    case AbbreviationRole:
        return rule.abbreviation;
    case ExpansionRole:
        return rule.expansion;
    default:
        return {};
    }
}

QHash<int, QByteArray> TextExpansionModel::roleNames() const
{
    return {
        {AbbreviationRole, "abbreviation"},
        {ExpansionRole, "expansion"},
    };
}

QVariantMap TextExpansionModel::ruleAt(int row) const
{
    if (row < 0 || row >= m_rules.size()) {
        return {};
    }
    return {
        {QStringLiteral("abbreviation"), m_rules.at(row).abbreviation},
        {QStringLiteral("expansion"), m_rules.at(row).expansion},
    };
}

bool TextExpansionModel::addRule(const QString &abbreviation, const QString &expansion)
{
    const QString normalizedAbbreviation = abbreviation.trimmed();
    if (normalizedAbbreviation.isEmpty() || expansion.isEmpty() || abbreviationExists(normalizedAbbreviation)) {
        return false;
    }

    const Rule rule{normalizedAbbreviation, expansion};
    const int row = std::lower_bound(m_rules.cbegin(), m_rules.cend(), rule, ruleLessThan) - m_rules.cbegin();
    beginInsertRows({}, row, row);
    m_rules.insert(row, rule);
    endInsertRows();
    Q_EMIT countChanged();
    saveRules();
    return true;
}

bool TextExpansionModel::updateRule(int row, const QString &abbreviation, const QString &expansion)
{
    const QString normalizedAbbreviation = abbreviation.trimmed();
    if (row < 0 || row >= m_rules.size() || normalizedAbbreviation.isEmpty() || expansion.isEmpty() || abbreviationExists(normalizedAbbreviation, row)) {
        return false;
    }

    const Rule rule{normalizedAbbreviation, expansion};
    const int insertionRow = std::lower_bound(m_rules.cbegin(), m_rules.cend(), rule, ruleLessThan) - m_rules.cbegin();
    const int newRow = insertionRow > row ? insertionRow - 1 : insertionRow;
    if (newRow != row) {
        beginMoveRows({}, row, row, {}, insertionRow);
        m_rules[row] = rule;
        m_rules.move(row, newRow);
        endMoveRows();
    } else {
        m_rules[row] = rule;
    }
    Q_EMIT dataChanged(index(newRow), index(newRow), {AbbreviationRole, ExpansionRole});
    saveRules();
    return true;
}

void TextExpansionModel::removeRule(int row)
{
    if (row < 0 || row >= m_rules.size()) {
        return;
    }

    beginRemoveRows({}, row, row);
    m_rules.removeAt(row);
    endRemoveRows();
    Q_EMIT countChanged();
    saveRules();
}

void TextExpansionModel::loadRules()
{
    const int oldCount = m_rules.size();
    beginResetModel();
    m_rules.clear();
    const QJsonDocument document = QJsonDocument::fromJson(m_settings->textExpansionRules().toUtf8());
    for (const QJsonValue &value : document.array()) {
        const QJsonObject object = value.toObject();
        const QString abbreviation = object.value(QStringLiteral("abbreviation")).toString().trimmed();
        const QString expansion = object.value(QStringLiteral("expansion")).toString();
        if (!abbreviation.isEmpty() && !expansion.isEmpty() && !abbreviationExists(abbreviation)) {
            m_rules.append({abbreviation, expansion});
        }
    }
    std::sort(m_rules.begin(), m_rules.end(), ruleLessThan);
    endResetModel();
    if (oldCount != m_rules.size()) {
        Q_EMIT countChanged();
    }
}

void TextExpansionModel::saveRules()
{
    QJsonArray array;
    for (const Rule &rule : m_rules) {
        const QJsonObject object{
            {QStringLiteral("abbreviation"), rule.abbreviation},
            {QStringLiteral("expansion"), rule.expansion},
        };
        array.append(QJsonValue(object));
    }
    const QScopedValueRollback writingRules(m_writingRules, true);
    m_settings->setTextExpansionRules(QString::fromUtf8(QJsonDocument(array).toJson(QJsonDocument::Compact)));
}

bool TextExpansionModel::abbreviationExists(const QString &abbreviation, int exceptRow) const
{
    for (int row = 0; row < m_rules.size(); ++row) {
        if (row != exceptRow && m_rules.at(row).abbreviation == abbreviation) {
            return true;
        }
    }
    return false;
}

bool TextExpansionModel::ruleLessThan(const Rule &left, const Rule &right)
{
    const int comparison = QString::compare(left.abbreviation, right.abbreviation, Qt::CaseInsensitive);
    return comparison == 0 ? left.abbreviation < right.abbreviation : comparison < 0;
}
