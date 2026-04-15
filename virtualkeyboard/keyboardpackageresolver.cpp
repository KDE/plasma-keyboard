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

QStringList KeyboardPackageResolver::keyboardLayoutIds() const
{
    return m_keyboardLayoutIds;
}

QString KeyboardPackageResolver::packageId(const QString &keyboardLayoutId) const
{
    const auto *layout = findKeyboardLayout(keyboardLayoutId);
    return layout ? layout->packageId : QString();
}

QString KeyboardPackageResolver::resolveKeyboardLayoutId(const QString &keyboardLayoutId) const
{
    const auto *layout = findKeyboardLayout(keyboardLayoutId);
    return layout ? layout->id : QString();
}

QString KeyboardPackageResolver::keyboardLayoutName(const QString &keyboardLayoutId) const
{
    const auto *layout = findKeyboardLayout(keyboardLayoutId);
    if (!layout) {
        return {};
    }

    const QString name = KeyboardLayoutMetadata::keyboardLayoutName(layout->definition);
    return name.isEmpty() ? KeyboardLayoutMetadata::keyboardPackageDisplayName(*layout) : name;
}

QString KeyboardPackageResolver::keyboardPackageName(const QString &keyboardLayoutId) const
{
    const auto *layout = findKeyboardLayout(keyboardLayoutId);
    return layout ? KeyboardLayoutMetadata::keyboardPackageDisplayName(*layout) : QString();
}

QString KeyboardPackageResolver::keyboardLayoutPrimaryLocale(const QString &keyboardLayoutId) const
{
    const auto *layout = findKeyboardLayout(keyboardLayoutId);
    return layout && !layout->locales.isEmpty() ? layout->locales.constFirst() : QString();
}

QStringList KeyboardPackageResolver::keyboardLayoutLocales(const QString &keyboardLayoutId) const
{
    const auto *layout = findKeyboardLayout(keyboardLayoutId);
    return layout ? layout->locales : QStringList();
}

QString KeyboardPackageResolver::layoutPath(const QString &keyboardLayoutId, const QString &layoutType) const
{
    const auto *layout = findKeyboardLayout(keyboardLayoutId);
    if (!layout || layoutType.isEmpty()) {
        return {};
    }

    const QString fileName = KeyboardLayoutMetadata::keyboardLayoutFile(layout->definition, layoutType);
    const QString layoutPath = layout->packagePath + u"/contents/layouts/"_s + fileName;
    if (QFileInfo::exists(layoutPath)) {
        return layoutPath;
    }
    return {};
}

QUrl KeyboardPackageResolver::layoutUrl(const QString &keyboardLayoutId, const QString &layoutType) const
{
    const QString path = layoutPath(keyboardLayoutId, layoutType);
    qCDebug(PlasmaKeyboard) << "KeyboardPackageResolver: loading keyboard layout" << keyboardLayoutId << layoutType << path;
    return path.isEmpty() ? QUrl() : QUrl::fromLocalFile(path);
}

void KeyboardPackageResolver::rebuildIndex()
{
    QList<KeyboardLayoutMetadata::Layout> keyboardLayouts = KeyboardLayoutMetadata::keyboardLayouts();
    for (const auto &layout : keyboardLayouts) {
        qCDebug(PlasmaKeyboard) << "KeyboardPackageResolver: discovered keyboard layout" << layout.id << layout.packageId << layout.packagePath;
    }

    QStringList ids;
    ids.reserve(keyboardLayouts.size());
    for (const auto &keyboardLayout : keyboardLayouts) {
        ids.append(keyboardLayout.id);
    }

    if (m_keyboardLayoutIds == ids && m_keyboardLayouts.size() == keyboardLayouts.size()) {
        return;
    }

    m_keyboardLayouts = keyboardLayouts;
    m_keyboardLayoutIds = ids;
    qCDebug(PlasmaKeyboard) << "KeyboardPackageResolver: active keyboard layouts" << m_keyboardLayoutIds;
    Q_EMIT keyboardLayoutIdsChanged();
}

const KeyboardLayoutMetadata::Layout *KeyboardPackageResolver::findKeyboardLayout(const QString &keyboardLayoutId) const
{
    for (const auto &layout : m_keyboardLayouts) {
        if (layout.id == keyboardLayoutId) {
            return &layout;
        }
    }

    for (const auto &layout : m_keyboardLayouts) {
        if (layout.packageId == keyboardLayoutId) {
            return &layout;
        }
    }

    return nullptr;
}
