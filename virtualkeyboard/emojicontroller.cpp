/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "emojicontroller.h"

#include "inmemoryinputbackend.h"
#include "inputengine.h"

EmojiController::EmojiController(InputEngine *inputEngine, QObject *parent)
    : QObject(parent)
    , m_inputEngine(inputEngine)
    , m_clientBackend(inputEngine->backend())
    , m_searchBackend(new InMemoryInputBackend(this))
{
    Q_ASSERT(m_inputEngine);

    connect(m_clientBackend, &InputBackend::inputMethodHintsChanged, this, [this] {
        Q_EMIT availableChanged();
        if (!available()) {
            close();
        }
    });
    connect(m_searchBackend, &InMemoryInputBackend::displayTextChanged, this, &EmojiController::queryChanged);
    connect(m_searchBackend, &InMemoryInputBackend::displayCursorPositionChanged, this, &EmojiController::queryCursorPositionChanged);
    connect(m_inputEngine, &InputEngine::backendChanged, this, &EmojiController::searchActiveChanged);
}

EmojiController::~EmojiController()
{
    if (m_inputEngine) {
        m_inputEngine->setBackend(m_clientBackend);
    }
}

bool EmojiController::active() const
{
    return m_active;
}

bool EmojiController::searchActive() const
{
    return m_inputEngine->backend() == m_searchBackend;
}

QString EmojiController::query() const
{
    return m_searchBackend->displayText();
}

int EmojiController::queryCursorPosition() const
{
    return m_searchBackend->displayCursorPosition();
}

bool EmojiController::available() const
{
    const auto unavailableHints = Qt::ImhHiddenText | Qt::ImhSensitiveData | Qt::ImhDate | Qt::ImhTime | Qt::ImhDigitsOnly | Qt::ImhFormattedNumbersOnly
        | Qt::ImhUppercaseOnly | Qt::ImhLowercaseOnly | Qt::ImhDialableCharactersOnly | Qt::ImhEmailCharactersOnly | Qt::ImhUrlCharactersOnly
        | Qt::ImhLatinOnly;
    return !m_clientBackend->inputMethodHints().testAnyFlags(unavailableHints);
}

void EmojiController::open()
{
    if (m_active) {
        // Return from emoji search to the emoji picker
        stopSearch();
        return;
    }
    if (!available()) {
        return;
    }

    m_inputEngine->commit();
    if (auto *composer = m_inputEngine->textComposer()) {
        composer->reset();
    }
    m_active = true;
    Q_EMIT activeChanged();
}

void EmojiController::close()
{
    if (!m_active) {
        return;
    }

    stopSearch();
    m_active = false;
    Q_EMIT activeChanged();
}

void EmojiController::startSearch()
{
    if (!m_active || searchActive()) {
        return;
    }

    m_inputEngine->setBackend(m_searchBackend);
}

void EmojiController::stopSearch()
{
    m_inputEngine->setBackend(m_clientBackend);
    clearQuery();
}

void EmojiController::clearQuery()
{
    if (searchActive()) {
        if (auto *composer = m_inputEngine->textComposer()) {
            composer->reset();
        }
    }
    m_searchBackend->clear();
}

void EmojiController::commitEmoji(const QString &emoji)
{
    if (!m_active || emoji.isEmpty()) {
        return;
    }

    m_clientBackend->commitText(emoji);
}
