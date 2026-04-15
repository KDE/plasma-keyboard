/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "keyboardcontroller.h"

#include "inputengine.h"
#include "keyboardlayoutmetadata.h"
#include "keyboardpackageresolver.h"

KeyboardController::KeyboardController(InputEngine *inputEngine, KeyboardPackageResolver *packageResolver, QObject *parent)
    : QObject(parent)
    , m_inputEngine(inputEngine)
    , m_packageResolver(packageResolver)
{
    Q_ASSERT(m_inputEngine);
    Q_ASSERT(m_packageResolver);

    connect(m_inputEngine, &InputEngine::inputMethodHintsChanged, this, &KeyboardController::layoutTypeChanged);
    connect(m_packageResolver, &KeyboardPackageResolver::keyboardLayoutIdsChanged, this, [this] {
        refreshLayoutId();
    });

    refreshLayoutId();
}

QString KeyboardController::layoutId() const
{
    return m_layoutId;
}

void KeyboardController::setLayoutId(const QString &layoutId)
{
    QString resolvedId = m_packageResolver->resolveKeyboardLayoutId(layoutId);
    if (!m_activeLayoutIds.isEmpty() && !m_activeLayoutIds.contains(resolvedId)) {
        resolvedId = m_activeLayoutIds.constFirst();
    }
    if (m_layoutId == resolvedId) {
        return;
    }

    m_layoutId = resolvedId;
    refreshLocale();
    Q_EMIT layoutIdChanged();
}

QStringList KeyboardController::activeLayoutIds() const
{
    return m_activeLayoutIds;
}

void KeyboardController::setActiveLayoutIds(const QStringList &activeLayoutIds)
{
    QStringList resolvedIds;
    const QStringList availableIds = m_packageResolver->keyboardLayoutIds();
    for (const auto &layoutId : activeLayoutIds) {
        const QString resolvedId = m_packageResolver->resolveKeyboardLayoutId(layoutId);
        if (availableIds.contains(resolvedId) && !resolvedIds.contains(resolvedId)) {
            resolvedIds.append(resolvedId);
        }
    }

    if (m_activeLayoutIds == resolvedIds) {
        return;
    }

    m_activeLayoutIds = resolvedIds;
    Q_EMIT activeLayoutIdsChanged();

    if (m_activeLayoutIds.isEmpty()) {
        refreshLayoutId();
        return;
    }

    if (!m_activeLayoutIds.contains(m_layoutId)) {
        setLayoutId(m_activeLayoutIds.constFirst());
        return;
    }

    refreshLocale();
}

int KeyboardController::currentLayoutIndex() const
{
    return m_activeLayoutIds.indexOf(m_layoutId);
}

bool KeyboardController::hasMultipleLayouts() const
{
    return m_activeLayoutIds.size() > 1;
}

bool KeyboardController::symbolMode() const
{
    return m_symbolMode;
}

void KeyboardController::setSymbolMode(bool symbolMode)
{
    if (m_symbolMode == symbolMode) {
        return;
    }

    m_symbolMode = symbolMode;
    Q_EMIT symbolModeChanged();
    Q_EMIT layoutTypeChanged();
}

QString KeyboardController::layoutType() const
{
    const auto hints = m_inputEngine->inputMethodHints();
    if (hints.testFlag(Qt::ImhDialableCharactersOnly)) {
        return QStringLiteral("dialpad");
    }
    if (hints.testFlag(Qt::ImhFormattedNumbersOnly)) {
        return QStringLiteral("numbers");
    }
    if (hints.testFlag(Qt::ImhDigitsOnly)) {
        return QStringLiteral("digits");
    }
    if (m_symbolMode) {
        return QStringLiteral("symbols");
    }
    return QStringLiteral("main");
}

void KeyboardController::cycleLayout()
{
    if (m_activeLayoutIds.size() <= 1) {
        return;
    }

    const int currentIndex = m_activeLayoutIds.indexOf(m_layoutId);
    const int nextIndex = currentIndex < 0 || currentIndex + 1 >= m_activeLayoutIds.size() ? 0 : currentIndex + 1;
    setLayoutId(m_activeLayoutIds.at(nextIndex));
}

void KeyboardController::setCurrentLayout(const QString &layoutId)
{
    if (layoutId.isEmpty()) {
        return;
    }

    if (!m_activeLayoutIds.isEmpty() && !m_activeLayoutIds.contains(layoutId)) {
        return;
    }

    setLayoutId(layoutId);
}

QString KeyboardController::layoutDisplayName(const QString &layoutId) const
{
    const QString name = m_packageResolver->keyboardPackageName(layoutId);
    if (!name.isEmpty()) {
        return name;
    }

    const QString locale = m_packageResolver->keyboardLayoutPrimaryLocale(layoutId);
    if (!locale.isEmpty()) {
        return KeyboardLayoutMetadata::localeDisplayName(locale);
    }

    return layoutId;
}

QString KeyboardController::layoutName(const QString &layoutId) const
{
    const QString name = m_packageResolver->keyboardLayoutName(layoutId);
    return name.isEmpty() ? layoutDisplayName(layoutId) : name;
}

QString KeyboardController::layoutPrimaryLocale(const QString &layoutId) const
{
    return m_packageResolver->keyboardLayoutPrimaryLocale(layoutId);
}

void KeyboardController::refreshLayoutId()
{
    if (!m_activeLayoutIds.isEmpty()) {
        setLayoutId(m_activeLayoutIds.contains(m_layoutId) ? m_layoutId : m_activeLayoutIds.constFirst());
        return;
    }

    setLayoutId(QString());
}

void KeyboardController::refreshLocale()
{
    const QString locale = layoutPrimaryLocale(m_layoutId);
    if (!locale.isEmpty() && m_inputEngine->locale() != locale) {
        m_inputEngine->setLocale(locale);
    }
}
