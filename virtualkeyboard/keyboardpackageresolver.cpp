/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "keyboardpackageresolver.h"
#include "logging.h"

#include <QFileInfo>

using namespace Qt::StringLiterals;

KeyboardPackageResolver::KeyboardPackageResolver(QObject *parent)
    : QObject(parent)
{
    rebuildIndex();
}

QStringList KeyboardPackageResolver::layoutIds() const
{
    return m_layoutIds;
}

QString KeyboardPackageResolver::packageId(const QString &layoutId) const
{
    const auto *layout = findLayout(layoutId);
    return layout ? layout->packageId : QString();
}

QString KeyboardPackageResolver::resolveLayoutId(const QString &id) const
{
    const auto *layout = findLayout(id);
    return layout ? layout->layoutId : QString();
}

QString KeyboardPackageResolver::layoutName(const QString &layoutId) const
{
    const auto *layout = findLayout(layoutId);
    if (!layout) {
        return {};
    }

    const QString name = KeyboardLayoutMetadata::keyboardLayoutName(layout->layoutDefinition);
    return name.isEmpty() ? KeyboardLayoutMetadata::keyboardDisplayName(*layout) : name;
}

QString KeyboardPackageResolver::keyboardName(const QString &layoutId) const
{
    const auto *layout = findLayout(layoutId);
    return layout ? KeyboardLayoutMetadata::keyboardDisplayName(*layout) : QString();
}

QString KeyboardPackageResolver::keyboardPrimaryLocale(const QString &layoutId) const
{
    const auto *layout = findLayout(layoutId);
    return layout && !layout->locales.isEmpty() ? layout->locales.constFirst() : QString();
}

QStringList KeyboardPackageResolver::keyboardLocales(const QString &layoutId) const
{
    const auto *layout = findLayout(layoutId);
    return layout ? layout->locales : QStringList();
}

QString KeyboardPackageResolver::layoutPath(const QString &layoutId, const QString &layoutType) const
{
    const auto *layout = findLayout(layoutId);
    if (!layout || layoutType.isEmpty()) {
        return {};
    }

    const QString fileName = KeyboardLayoutMetadata::keyboardLayoutFile(layout->layoutDefinition, layoutType);
    const QString layoutPath = layout->packagePath + u"/contents/layouts/"_s + fileName;
    if (QFileInfo::exists(layoutPath)) {
        return layoutPath;
    }
    return {};
}

QUrl KeyboardPackageResolver::layoutUrl(const QString &layoutId, const QString &layoutType) const
{
    const QString path = layoutPath(layoutId, layoutType);
    qCDebug(PlasmaKeyboard) << "KeyboardPackageResolver: loading keyboard layout" << layoutId << layoutType << path;
    return path.isEmpty() ? QUrl() : QUrl::fromLocalFile(path);
}

void KeyboardPackageResolver::rebuildIndex()
{
    QList<KeyboardLayoutMetadata::Layout> layouts = KeyboardLayoutMetadata::keyboardLayouts();
    for (const auto &layout : layouts) {
        qCDebug(PlasmaKeyboard) << "KeyboardPackageResolver: discovered keyboard layout" << layout.layoutId << layout.packageId << layout.packagePath;
    }

    QStringList ids;
    ids.reserve(layouts.size());
    for (const auto &layout : layouts) {
        ids.append(layout.layoutId);
    }

    if (m_layoutIds == ids && m_layouts.size() == layouts.size()) {
        return;
    }

    m_layouts = layouts;
    m_layoutIds = ids;
    qCDebug(PlasmaKeyboard) << "KeyboardPackageResolver: active keyboard layouts" << m_layoutIds;
    Q_EMIT layoutIdsChanged();
}

const KeyboardLayoutMetadata::Layout *KeyboardPackageResolver::findLayout(const QString &id) const
{
    for (const auto &layout : m_layouts) {
        if (layout.layoutId == id) {
            return &layout;
        }
    }

    for (const auto &layout : m_layouts) {
        if (layout.packageId == id) {
            return &layout;
        }
    }

    return nullptr;
}
