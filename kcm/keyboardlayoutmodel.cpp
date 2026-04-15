/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "keyboardlayoutmodel.h"

#include <QLocale>

KeyboardLayoutModel::KeyboardLayoutModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int KeyboardLayoutModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_layouts.size();
}

QVariant KeyboardLayoutModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_layouts.size()) {
        return {};
    }

    const Layout &layout = m_layouts.at(index.row());
    switch (role) {
    case LayoutIdRole:
        return layout.layoutId;
    case NameRole:
        return layout.name;
    case DescriptionRole:
        return layout.description;
    case VisibleRole:
        return matchesFilter(layout, m_filterText);
    case EnabledRole:
        return m_enabledLayoutIds.contains(layout.layoutId);
    default:
        return {};
    }
}

QHash<int, QByteArray> KeyboardLayoutModel::roleNames() const
{
    return {
        {LayoutIdRole, "layoutId"},
        {NameRole, "name"},
        {DescriptionRole, "description"},
        {VisibleRole, "matchesFilter"},
        {EnabledRole, "enabled"},
    };
}

void KeyboardLayoutModel::setFilterText(const QString &filterText)
{
    const QString normalizedFilter = filterText.toLower();
    if (m_filterText == normalizedFilter) {
        return;
    }

    m_filterText = normalizedFilter;
    emitDataChanged();
}

void KeyboardLayoutModel::setEnabledLayoutIds(const QStringList &enabledLayoutIds)
{
    if (m_enabledLayoutIds == enabledLayoutIds) {
        return;
    }

    m_enabledLayoutIds = enabledLayoutIds;
    emitDataChanged();
}

int KeyboardLayoutModel::visibleLayoutCount() const
{
    int count = 0;
    for (const auto &layout : m_layouts) {
        if (matchesFilter(layout, m_filterText)) {
            ++count;
        }
    }
    return count;
}

int KeyboardLayoutModel::enabledVisibleLayoutCount() const
{
    int count = 0;
    for (const auto &layout : m_layouts) {
        if (matchesFilter(layout, m_filterText) && m_enabledLayoutIds.contains(layout.layoutId)) {
            ++count;
        }
    }
    return count;
}

QString KeyboardLayoutModel::layoutForLocale(const QString &locale) const
{
    const QString languageCode = locale.section(QLatin1Char('_'), 0, 0);

    for (const auto &layout : m_layouts) {
        if (layout.locales.contains(locale)) {
            return layout.layoutId;
        }
    }

    for (const auto &layout : m_layouts) {
        for (const auto &candidateLocale : layout.locales) {
            if (candidateLocale.section(QLatin1Char('_'), 0, 0) == languageCode) {
                return layout.layoutId;
            }
        }
    }

    return QStringLiteral("org.kde.plasma.keyboard.en/qwerty");
}

void KeyboardLayoutModel::emitDataChanged()
{
    if (m_layouts.isEmpty()) {
        return;
    }

    Q_EMIT dataChanged(index(0), index(m_layouts.size() - 1), {VisibleRole, EnabledRole});
}

void KeyboardLayoutModel::setLayouts(QList<Layout> layouts)
{
    beginResetModel();
    m_layouts = std::move(layouts);
    endResetModel();
}

bool KeyboardLayoutModel::matchesFilter(const Layout &layout, const QString &filterText)
{
    return filterText.isEmpty() || layout.name.toLower().contains(filterText) || layout.description.toLower().contains(filterText);
}

QString KeyboardLayoutModel::descriptionForLocales(const QStringList &locales)
{
    QStringList names;
    names.reserve(locales.size());

    for (const auto &locale : locales) {
        const QLocale qlocale(locale);
        const QString language = qlocale.nativeLanguageName();
        const QString territory = qlocale.nativeTerritoryName();
        names.append(territory.isEmpty() ? language : language + QStringLiteral(" (") + territory + QStringLiteral(")"));
    }

    return names.join(QStringLiteral(", "));
}
