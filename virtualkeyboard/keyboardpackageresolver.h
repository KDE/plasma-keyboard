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

/**
 * Utility to find and parse keyboard "layout packages".
 */
class KeyboardPackageResolver : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
    Q_PROPERTY(QStringList layoutIds READ layoutIds NOTIFY layoutIdsChanged)

public:
    /**
     * Creates a resolver and indexes installed layout packages.
     */
    explicit KeyboardPackageResolver(QObject *parent = nullptr);

    /**
     * Returns the installed layout IDs.
     */
    QStringList layoutIds() const;

    /**
     * Returns the package ID containing @p layoutId.
     * Package ID example: org.kde.plasma.keyboard.en
     *
     * The argument may be a layout ID or a package ID.
     */
    Q_INVOKABLE QString packageId(const QString &layoutId) const;

    /**
     * Returns the layout ID for @p id.
     * Layout ID example: org.kde.plasma.keyboard.en/mobile-qwerty
     *
     * A package ID resolves to its first layout.
     */
    Q_INVOKABLE QString resolveLayoutId(const QString &id) const;

    /**
     * Returns the display name of @p layoutId.
     */
    Q_INVOKABLE QString layoutName(const QString &layoutId) const;

    /**
     * Returns the keyboard name for @p layoutId.
     */
    Q_INVOKABLE QString keyboardName(const QString &layoutId) const;

    /**
     * Returns the primary locale of the keyboard containing @p layoutId.
     */
    Q_INVOKABLE QString keyboardPrimaryLocale(const QString &layoutId) const;

    /**
     * Returns the locales supported by the keyboard containing @p layoutId.
     */
    Q_INVOKABLE QStringList keyboardLocales(const QString &layoutId) const;

    /**
     * Returns the local QML file URL for @p layoutId and @p layoutType.
     */
    Q_INVOKABLE QUrl layoutUrl(const QString &layoutId, const QString &layoutType) const;

Q_SIGNALS:
    /**
     * Emitted when the installed layout IDs change.
     */
    void layoutIdsChanged();

private:
    void rebuildIndex();
    QString layoutPath(const QString &layoutId, const QString &layoutType) const;
    const KeyboardLayoutMetadata::Layout *findLayout(const QString &id) const;

    QStringList m_layoutIds;
    QList<KeyboardLayoutMetadata::Layout> m_layouts;
};
