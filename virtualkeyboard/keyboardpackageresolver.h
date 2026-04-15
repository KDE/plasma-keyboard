/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QStringList>
#include <QUrl>
#include <qqmlintegration.h>

#include "keyboardlayoutmetadata.h"

class KeyboardPackageResolver : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
    Q_PROPERTY(QStringList keyboardLayoutIds READ keyboardLayoutIds NOTIFY keyboardLayoutIdsChanged)

public:
    explicit KeyboardPackageResolver(QObject *parent = nullptr);

    QStringList keyboardLayoutIds() const;

    Q_INVOKABLE QString packageId(const QString &keyboardLayoutId) const;
    Q_INVOKABLE QString resolveKeyboardLayoutId(const QString &keyboardLayoutId) const;
    Q_INVOKABLE QString keyboardLayoutName(const QString &keyboardLayoutId) const;
    Q_INVOKABLE QString keyboardPackageName(const QString &keyboardLayoutId) const;
    Q_INVOKABLE QString keyboardLayoutPrimaryLocale(const QString &keyboardLayoutId) const;
    Q_INVOKABLE QStringList keyboardLayoutLocales(const QString &keyboardLayoutId) const;
    Q_INVOKABLE QUrl layoutUrl(const QString &keyboardLayoutId, const QString &layoutType) const;

Q_SIGNALS:
    void keyboardLayoutIdsChanged();

private:
    void rebuildIndex();
    QString layoutPath(const QString &keyboardLayoutId, const QString &layoutType) const;
    const KeyboardLayoutMetadata::Layout *findKeyboardLayout(const QString &keyboardLayoutId) const;

    QStringList m_keyboardLayoutIds;
    QList<KeyboardLayoutMetadata::Layout> m_keyboardLayouts;
};
