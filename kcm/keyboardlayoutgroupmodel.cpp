/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "keyboardlayoutgroupmodel.h"

#include "keyboardlayoutmetadata.h"

#include <QHash>
#include <algorithm>

KeyboardLayoutGroupModel::KeyboardLayoutGroupModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_flatLayouts(new KeyboardLayoutModel(this))
{
}

int KeyboardLayoutGroupModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_groups.size();
}

QVariant KeyboardLayoutGroupModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_groups.size()) {
        return {};
    }

    const Group &group = m_groups.at(index.row());
    switch (role) {
    case GroupIdRole:
        return group.groupId;
    case NameRole:
        return group.name;
    case DescriptionRole:
        return group.description;
    case LayoutsRole:
        return QVariant::fromValue(group.layouts);
    case VisibleRole:
        return group.layouts && group.layouts->visibleLayoutCount() > 0;
    case DisplayExpandedRole:
        return m_expandedGroupIds.contains(group.groupId) || !m_filterText.isEmpty();
    case SelectedCountRole:
        return group.layouts ? group.layouts->enabledVisibleLayoutCount() : 0;
    case VisibleLayoutCountRole:
        return group.layouts ? group.layouts->visibleLayoutCount() : 0;
    default:
        return {};
    }
}

QHash<int, QByteArray> KeyboardLayoutGroupModel::roleNames() const
{
    return {
        {GroupIdRole, "groupId"},
        {NameRole, "name"},
        {DescriptionRole, "description"},
        {LayoutsRole, "layouts"},
        {VisibleRole, "matchesFilter"},
        {DisplayExpandedRole, "displayExpanded"},
        {SelectedCountRole, "selectedCount"},
        {VisibleLayoutCountRole, "visibleLayoutCount"},
    };
}

KeyboardLayoutModel *KeyboardLayoutGroupModel::layouts() const
{
    return m_flatLayouts;
}

QString KeyboardLayoutGroupModel::layoutForLocale(const QString &locale) const
{
    return m_flatLayouts->layoutForLocale(locale);
}

QString KeyboardLayoutGroupModel::filterText() const
{
    return m_filterText;
}

void KeyboardLayoutGroupModel::setFilterText(const QString &filterText)
{
    const QString normalizedFilter = filterText.toLower();
    if (m_filterText == normalizedFilter) {
        return;
    }

    m_filterText = normalizedFilter;
    refreshChildModels();
    Q_EMIT filterTextChanged();
    emitDataChanged();
}

void KeyboardLayoutGroupModel::setEnabledLayoutIds(const QStringList &enabledLayoutIds)
{
    if (m_enabledLayoutIds == enabledLayoutIds) {
        return;
    }

    m_enabledLayoutIds = enabledLayoutIds;
    refreshChildModels();
    emitDataChanged();
}

void KeyboardLayoutGroupModel::toggleExpanded(const QString &groupId)
{
    const int index = m_expandedGroupIds.indexOf(groupId);
    if (index == -1) {
        m_expandedGroupIds.append(groupId);
    } else {
        m_expandedGroupIds.removeAt(index);
    }
    emitDataChanged();
}

void KeyboardLayoutGroupModel::setFormFactorFilter(const QStringList &formFactorFilter)
{
    if (m_formFactorFilter == formFactorFilter) {
        return;
    }

    m_formFactorFilter = formFactorFilter;
    loadInstalledGroups();
}

void KeyboardLayoutGroupModel::loadInstalledGroups()
{
    beginResetModel();
    clearGroups();

    QHash<QString, int> groupIndexByPackageId;
    QList<KeyboardLayoutModel::Layout> flatLayouts;
    QList<QList<KeyboardLayoutModel::Layout>> groupedLayouts;

    for (const auto &entry : KeyboardLayoutMetadata::keyboardLayouts()) {
        if (!matchesFormFactor(entry.formFactors)) {
            continue;
        }

        const QString layoutName = KeyboardLayoutMetadata::keyboardLayoutName(entry.definition);
        const QString packageName = KeyboardLayoutMetadata::keyboardPackageDisplayName(entry);
        const QString displayName = layoutName.isEmpty() ? packageName : packageName + QStringLiteral(" - ") + layoutName;

        flatLayouts.append({
            .layoutId = entry.id,
            .name = displayName,
            .description = KeyboardLayoutModel::descriptionForLocales(entry.locales),
            .locales = entry.locales,
        });

        if (!groupIndexByPackageId.contains(entry.packageId)) {
            const QString localeDescription = KeyboardLayoutModel::descriptionForLocales(entry.locales);
            groupIndexByPackageId.insert(entry.packageId, m_groups.size());
            groupedLayouts.append(QList<KeyboardLayoutModel::Layout>{});
            m_groups.append({
                .groupId = entry.packageId,
                .name = entry.packageName.isEmpty() ? localeDescription : entry.packageName,
                .description = localeDescription,
            });
        }

        groupedLayouts[groupIndexByPackageId.value(entry.packageId)].append({
            .layoutId = entry.id,
            .name = layoutName.isEmpty() ? displayName : layoutName,
            .description = entry.id,
            .locales = entry.locales,
        });
    }

    for (qsizetype i = 0; i < m_groups.size(); ++i) {
        QList<KeyboardLayoutModel::Layout> layouts = std::move(groupedLayouts[i]);
        std::sort(layouts.begin(), layouts.end(), [](const auto &left, const auto &right) {
            return left.name.localeAwareCompare(right.name) < 0;
        });
        auto *layoutModel = new KeyboardLayoutModel(this);
        layoutModel->setLayouts(std::move(layouts));
        m_groups[i].layouts = layoutModel;
    }
    std::sort(m_groups.begin(), m_groups.end(), [](const auto &left, const auto &right) {
        return left.name.localeAwareCompare(right.name) < 0;
    });

    std::sort(flatLayouts.begin(), flatLayouts.end(), [](const auto &left, const auto &right) {
        return left.name.localeAwareCompare(right.name) < 0;
    });
    m_flatLayouts->setLayouts(std::move(flatLayouts));
    refreshChildModels();
    endResetModel();
}

void KeyboardLayoutGroupModel::clearGroups()
{
    for (const auto &group : std::as_const(m_groups)) {
        delete group.layouts;
    }
    m_groups.clear();
}

bool KeyboardLayoutGroupModel::matchesFormFactor(const QStringList &formFactors) const
{
    if (m_formFactorFilter.isEmpty() || formFactors.isEmpty()) {
        return true;
    }

    for (const QString &formFactor : m_formFactorFilter) {
        if (formFactors.contains(formFactor)) {
            return true;
        }
    }

    return false;
}

bool KeyboardLayoutGroupModel::groupNameMatches(const Group &group) const
{
    return m_filterText.isEmpty() || group.name.toLower().contains(m_filterText) || group.description.toLower().contains(m_filterText);
}

void KeyboardLayoutGroupModel::refreshChildModels()
{
    m_flatLayouts->setEnabledLayoutIds(m_enabledLayoutIds);
    m_flatLayouts->setFilterText(m_filterText);
    for (const auto &group : std::as_const(m_groups)) {
        if (!group.layouts) {
            continue;
        }
        group.layouts->setEnabledLayoutIds(m_enabledLayoutIds);
        group.layouts->setFilterText(groupNameMatches(group) ? QString() : m_filterText);
    }
}

void KeyboardLayoutGroupModel::emitDataChanged()
{
    if (m_groups.isEmpty()) {
        return;
    }

    Q_EMIT dataChanged(index(0), index(m_groups.size() - 1), {VisibleRole, DisplayExpandedRole, SelectedCountRole, VisibleLayoutCountRole});
}
