/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "pinyintextcomposer.h"

#include "../inputengine.h"
#include "config-plasma-keyboard.h"
#include "logging.h"

#include <QDir>
#include <QStandardPaths>

#include <pinyin.h>

static pinyin_context_t *sharedPinyinContext()
{
    static pinyin_context_t *context = []() -> pinyin_context_t * {
        const QString userDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/libpinyin");
        QDir().mkpath(userDir);

        const QString systemDir = QString::fromUtf8(PLASMA_KEYBOARD_LIBPINYIN_SYSTEM_DIR);
        pinyin_context_t *ctx = pinyin_init(systemDir.toUtf8().constData(), userDir.toUtf8().constData());
        if (!ctx) {
            qCWarning(PlasmaKeyboard) << "PinyinTextComposer: failed to initialize libpinyin context with system directory" << systemDir;
            return nullptr;
        }

        pinyin_set_full_pinyin_scheme(ctx, FULL_PINYIN_DEFAULT);
        pinyin_set_options(ctx, USE_DIVIDED_TABLE | USE_RESPLIT_TABLE | PINYIN_INCOMPLETE | DYNAMIC_ADJUST);
        for (guint8 index = GB_DICTIONARY; index <= ADDON_DICTIONARY; ++index) {
            pinyin_load_phrase_library(ctx, index);
        }
        return ctx;
    }();

    return context;
}

PinyinTextComposer::PinyinTextComposer(QObject *parent)
    : AbstractTextComposer(parent)
{
}

QString PinyinTextComposer::inputMode() const
{
    return m_inputMode;
}

void PinyinTextComposer::setInputMode(const QString &inputMode)
{
    if (m_inputMode == inputMode || inputMode.isEmpty()) {
        return;
    }

    reset();
    m_inputMode = inputMode;
    Q_EMIT inputModeChanged();
}

bool PinyinTextComposer::setTextCase(TextCase textCase)
{
    m_textCase = textCase;
    return true;
}

bool PinyinTextComposer::keyEvent(Qt::Key key, const QString &text)
{
    Q_UNUSED(m_textCase)

    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    if (inputMode() == QStringLiteral("latin")) {
        return engine->handleKeyCommit(key, text);
    }

    if (!m_surface.isEmpty()) {
        switch (key) {
        case Qt::Key_Backspace:
            m_surface.chop(1);
            refreshLookup();
            return true;
        case Qt::Key_Left:
            return false;
        case Qt::Key_Right:
            return false;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            return commitSurface();
        case Qt::Key_Space:
            return commitCurrentSelection();
        case Qt::Key_Escape:
            reset();
            return true;
        default:
            break;
        }

        if (isPinyinInput(text)) {
            m_surface += text.toLower();
            refreshLookup();
            return true;
        }

        const bool committed = commitCurrentSelection();
        return engine->handleKeyCommit(key, text) || committed;
    }

    if (isPinyinInput(text)) {
        m_surface += text.toLower();
        refreshLookup();
        return true;
    }

    return engine->handleKeyCommit(key, text);
}

bool PinyinTextComposer::selectCandidate(int index)
{
    const QStringList currentCandidates = candidates();
    if (index < 0 || index >= currentCandidates.size()) {
        return false;
    }

    return commitCurrentSelection(currentCandidates.at(index));
}

bool PinyinTextComposer::showsPreeditBubble() const
{
    return true;
}

void PinyinTextComposer::reset()
{
    m_surface.clear();
    clearComposition();
}

void PinyinTextComposer::update()
{
    refreshLookup();
}

bool PinyinTextComposer::commitCurrentSelection(const QString &text)
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    QString commitText = text;
    if (commitText.isEmpty()) {
        const QStringList currentCandidates = candidates();
        if (!currentCandidates.isEmpty()) {
            commitText = currentCandidates.constFirst();
        } else {
            commitText = m_surface;
        }
    }

    if (commitText.isEmpty()) {
        return false;
    }

    engine->commit(commitText);
    m_surface.clear();
    return true;
}

bool PinyinTextComposer::commitSurface()
{
    auto *engine = inputEngine();
    if (!engine || m_surface.isEmpty()) {
        return false;
    }

    engine->commit(m_surface);
    m_surface.clear();
    return true;
}

bool PinyinTextComposer::isPinyinInput(const QString &text) const
{
    return text.size() == 1 && (text.at(0).isLetter() || text == QStringLiteral("'"));
}

void PinyinTextComposer::refreshLookup()
{
    if (m_surface.isEmpty()) {
        clearComposition();
        return;
    }

    if (!rebuildLookup()) {
        setPreeditText(m_surface);
        setCandidates({m_surface});
        return;
    }
}

bool PinyinTextComposer::rebuildLookup()
{
    pinyin_context_t *context = sharedPinyinContext();
    if (!context) {
        return false;
    }

    pinyin_instance_t *instance = pinyin_alloc_instance(context);
    if (!instance) {
        return false;
    }

    const QByteArray surfaceUtf8 = m_surface.toUtf8();
    pinyin_parse_more_full_pinyins(instance, surfaceUtf8.constData());
    const qsizetype parsedLength = qsizetype(pinyin_get_parsed_input_length(instance));
    QStringList candidateList;
    const QString preedit = m_surface;

    if (pinyin_guess_sentence(instance)) {
        char *sentenceUtf8 = nullptr;
        if (pinyin_get_sentence(instance, 0, &sentenceUtf8) && sentenceUtf8) {
            QString sentence = QString::fromUtf8(sentenceUtf8);
            g_free(sentenceUtf8);
            if (parsedLength < m_surface.size()) {
                sentence += m_surface.mid(parsedLength);
            }
            if (!sentence.isEmpty()) {
                candidateList.append(sentence);
            }
        }
    }

    if (pinyin_guess_candidates(instance, 0, SORT_BY_PHRASE_LENGTH_AND_FREQUENCY)) {
        guint candidateCount = 0;
        if (pinyin_get_n_candidate(instance, &candidateCount)) {
            const guint maxCandidates = qMin(candidateCount, 32U);
            for (guint i = 0; i < maxCandidates; ++i) {
                lookup_candidate_t *candidate = nullptr;
                const gchar *candidateUtf8 = nullptr;
                if (!pinyin_get_candidate(instance, i, &candidate) || !candidate) {
                    continue;
                }
                if (!pinyin_get_candidate_string(instance, candidate, &candidateUtf8) || !candidateUtf8) {
                    continue;
                }

                QString candidateText = QString::fromUtf8(candidateUtf8);
                if (parsedLength < m_surface.size()) {
                    candidateText += m_surface.mid(parsedLength);
                }
                if (!candidateText.isEmpty()) {
                    candidateList.append(candidateText);
                }
            }
        }
    }
    candidateList.removeDuplicates();

    if (candidateList.isEmpty()) {
        if (!preedit.isEmpty()) {
            candidateList.append(preedit);
        }
    }

    setPreeditPrefix(QString());
    setPreeditText(preedit);
    setCandidates(candidateList);
    pinyin_free_instance(instance);
    return true;
}
