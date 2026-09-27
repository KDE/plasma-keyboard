/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "emojicontroller.h"

#include "inputengine.h"

#include <QTextBoundaryFinder>

EmojiController::EmojiController(InputEngine *inputEngine, QObject *parent)
    : QObject(parent)
    , m_inputEngine(inputEngine)
{
    Q_ASSERT(m_inputEngine);

    connect(m_inputEngine, &InputEngine::inputMethodHintsChanged, this, [this] {
        Q_EMIT availableChanged();
        if (!available()) {
            close();
        }
    });
    connect(m_inputEngine, &InputEngine::preeditTextChanged, this, [this] {
        if (m_searchActive) {
            Q_EMIT queryChanged();
        }
    });
    connect(m_inputEngine, &InputEngine::textCaptured, this, [this](const QString &text, int replaceFrom, int replaceLength) {
        if (m_searchActive) {
            replaceQueryText(text, replaceFrom, replaceLength);
        }
    });
    connect(m_inputEngine, &InputEngine::capturedBackspaceRequested, this, [this] {
        if (m_searchActive) {
            removeLastQueryCharacter();
        }
    });
}

EmojiController::~EmojiController()
{
    stopSearch();
}

bool EmojiController::active() const
{
    return m_active;
}

bool EmojiController::searchActive() const
{
    return m_searchActive;
}

QString EmojiController::query() const
{
    return m_query + (m_searchActive ? m_inputEngine->preeditText() : QString());
}

bool EmojiController::available() const
{
    const auto unavailableHints = Qt::ImhHiddenText | Qt::ImhSensitiveData | Qt::ImhDate | Qt::ImhTime | Qt::ImhDigitsOnly | Qt::ImhFormattedNumbersOnly
        | Qt::ImhUppercaseOnly | Qt::ImhLowercaseOnly | Qt::ImhDialableCharactersOnly | Qt::ImhEmailCharactersOnly | Qt::ImhUrlCharactersOnly
        | Qt::ImhLatinOnly;
    return !m_inputEngine->inputMethodHints().testAnyFlags(unavailableHints);
}

void EmojiController::open()
{
    if (m_active || !available()) {
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
    if (!m_active || m_searchActive) {
        return;
    }

    m_inputEngine->beginTextCapture();
    m_searchActive = true;
    Q_EMIT searchActiveChanged();
}

void EmojiController::stopSearch()
{
    if (!m_searchActive) {
        clearQuery();
        return;
    }

    if (auto *composer = m_inputEngine->textComposer()) {
        composer->reset();
    }
    m_inputEngine->endTextCapture();
    m_searchActive = false;
    Q_EMIT searchActiveChanged();
    clearQuery();
}

void EmojiController::clearQuery()
{
    if (m_searchActive) {
        if (auto *composer = m_inputEngine->textComposer()) {
            composer->reset();
        }
    }
    setQuery(QString());
}

void EmojiController::commitEmoji(const QString &emoji)
{
    if (!m_active || emoji.isEmpty()) {
        return;
    }

    if (auto *composer = m_inputEngine->textComposer()) {
        composer->reset();
    }
    m_inputEngine->commitDirect(emoji);
}

void EmojiController::appendQueryText(const QString &text)
{
    if (!m_searchActive || text.isEmpty()) {
        return;
    }

    setQuery(m_query + text);
}

void EmojiController::removeLastQueryCharacter()
{
    if (!m_searchActive || m_query.isEmpty()) {
        return;
    }

    QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, m_query);
    finder.toEnd();
    const qsizetype previousBoundary = finder.toPreviousBoundary();
    setQuery(previousBoundary < 0 ? QString() : m_query.first(previousBoundary));
}

void EmojiController::setQuery(const QString &query)
{
    if (m_query == query) {
        return;
    }

    m_query = query;
    Q_EMIT queryChanged();
}

void EmojiController::replaceQueryText(const QString &text, int replaceFrom, int replaceLength)
{
    if (!m_searchActive) {
        return;
    }

    QString query = m_query;
    if (replaceLength > 0) {
        int start = replaceFrom < 0 ? query.size() + replaceFrom : replaceFrom;
        start = qBound(0, start, query.size());
        query.remove(start, qMin(replaceLength, query.size() - start));
    }
    query.append(text);
    setQuery(query);
}
