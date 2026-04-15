/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QStringList>
#include <qqmlintegration.h>

class InputEngine;
class KeyboardPackageResolver;

class KeyboardController : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
    Q_PROPERTY(QString layoutId READ layoutId WRITE setLayoutId NOTIFY layoutIdChanged)
    Q_PROPERTY(QStringList activeLayoutIds READ activeLayoutIds WRITE setActiveLayoutIds NOTIFY activeLayoutIdsChanged)
    Q_PROPERTY(int currentLayoutIndex READ currentLayoutIndex NOTIFY layoutIdChanged)
    Q_PROPERTY(bool hasMultipleLayouts READ hasMultipleLayouts NOTIFY activeLayoutIdsChanged)
    Q_PROPERTY(bool symbolMode READ symbolMode WRITE setSymbolMode NOTIFY symbolModeChanged)
    Q_PROPERTY(QString layoutType READ layoutType NOTIFY layoutTypeChanged)

public:
    explicit KeyboardController(InputEngine *inputEngine, KeyboardPackageResolver *packageResolver, QObject *parent = nullptr);

    QString layoutId() const;
    void setLayoutId(const QString &layoutId);

    QStringList activeLayoutIds() const;
    void setActiveLayoutIds(const QStringList &activeLayoutIds);
    int currentLayoutIndex() const;
    bool hasMultipleLayouts() const;

    bool symbolMode() const;
    void setSymbolMode(bool symbolMode);

    QString layoutType() const;

    Q_INVOKABLE void cycleLayout();
    Q_INVOKABLE void setCurrentLayout(const QString &layoutId);
    Q_INVOKABLE QString layoutDisplayName(const QString &layoutId) const;
    Q_INVOKABLE QString layoutName(const QString &layoutId) const;

Q_SIGNALS:
    void layoutIdChanged();
    void activeLayoutIdsChanged();
    void symbolModeChanged();
    void layoutTypeChanged();

private:
    void refreshLayoutId();
    void refreshLocale();
    QString layoutPrimaryLocale(const QString &layoutId) const;

    InputEngine *m_inputEngine = nullptr;
    KeyboardPackageResolver *m_packageResolver = nullptr;
    QString m_layoutId;
    QStringList m_activeLayoutIds;
    bool m_symbolMode = false;
};
