/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "enabledkeyboardlayoutmodel.h"

#include "keyboardlayoutmetadata.h"

EnabledKeyboardLayoutModel::EnabledKeyboardLayoutModel(QObject *parent)
    : QAbstractListModel(parent)
{
    for (const auto &entry : KeyboardLayoutMetadata::keyboardLayouts()) {
        m_installedLayouts.insert(entry.layoutId,
                                  {
                                      .layoutId = entry.layoutId,
                                      .name = KeyboardLayoutModel::displayName(entry),
                                      .description = KeyboardLayoutModel::descriptionForLocales(entry.locales),
                                      .locales = entry.locales,
                                  });
    }
}

int EnabledKeyboardLayoutModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_layoutIds.size();
}

QVariant EnabledKeyboardLayoutModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_layoutIds.size()) {
        return {};
    }

    QString layoutId = m_layoutIds.at(index.row());
    const auto it = m_installedLayouts.constFind(layoutId);
    const bool installed = it != m_installedLayouts.constEnd();

    switch (role) {
    case LayoutIdRole:
        return layoutId;
    case NameRole:
        return installed ? it->name : layoutId;
    default:
        return {};
    }
}

QHash<int, QByteArray> EnabledKeyboardLayoutModel::roleNames() const
{
    return {
        {LayoutIdRole, "layoutId"},
        {NameRole, "name"},
    };
}

QStringList EnabledKeyboardLayoutModel::layoutIds() const
{
    return m_layoutIds;
}

void EnabledKeyboardLayoutModel::setLayoutIds(const QStringList &layoutIds)
{
    if (m_layoutIds == layoutIds) {
        return;
    }

    beginResetModel();
    m_layoutIds = layoutIds;
    endResetModel();
}

bool EnabledKeyboardLayoutModel::move(int fromRow, int destRow)
{
    if (fromRow == destRow || fromRow < 0 || destRow < 0 || fromRow >= m_layoutIds.size() || destRow >= m_layoutIds.size()) {
        return false;
    }

    if (!beginMoveRows({}, fromRow, fromRow, {}, destRow > fromRow ? destRow + 1 : destRow)) {
        return false;
    }
    m_layoutIds.move(fromRow, destRow);
    endMoveRows();
    return true;
}
