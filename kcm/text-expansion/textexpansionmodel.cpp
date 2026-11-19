/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "textexpansionmodel.h"

#include "plasmakeyboardsettings.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

TextExpansionModel::TextExpansionModel(PlasmaKeyboardSettings *settings, QObject *parent)
    : QAbstractListModel(parent)
    , m_settings(settings)
{
    loadRules();
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

    const int row = m_rules.size();
    beginInsertRows({}, row, row);
    m_rules.append({normalizedAbbreviation, expansion});
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

    m_rules[row] = {normalizedAbbreviation, expansion};
    Q_EMIT dataChanged(index(row), index(row), {AbbreviationRole, ExpansionRole});
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
    const QJsonDocument document = QJsonDocument::fromJson(m_settings->textExpansionRules().toUtf8());
    for (const QJsonValue &value : document.array()) {
        const QJsonObject object = value.toObject();
        const QString abbreviation = object.value(QStringLiteral("abbreviation")).toString().trimmed();
        const QString expansion = object.value(QStringLiteral("expansion")).toString();
        if (!abbreviation.isEmpty() && !expansion.isEmpty() && !abbreviationExists(abbreviation)) {
            m_rules.append({abbreviation, expansion});
        }
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
