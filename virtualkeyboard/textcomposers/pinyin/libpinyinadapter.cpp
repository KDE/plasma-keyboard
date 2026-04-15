/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "libpinyinadapter.h"

#include "config-plasma-keyboard.h"
#include "logging.h"

#include <QDir>
#include <QStandardPaths>

#include <pinyin.h>

static uint CANDIDATES_LIMIT = 64;

static pinyin_context_t *sharedPinyinContext()
{
    static pinyin_context_t *context = []() -> pinyin_context_t * {
        const QString userDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/libpinyin");
        QDir().mkpath(userDir);

        const QString systemDir = QString::fromUtf8(PLASMA_KEYBOARD_LIBPINYIN_SYSTEM_DIR);
        pinyin_context_t *ctx = pinyin_init(systemDir.toUtf8().constData(), userDir.toUtf8().constData());
        if (!ctx) {
            qCWarning(PlasmaKeyboard) << "LibPinyinAdapter: failed to initialize libpinyin context with system directory" << systemDir;
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

LibPinyinAdapter::~LibPinyinAdapter()
{
    if (m_instance) {
        pinyin_free_instance(m_instance);
    }
}

bool LibPinyinAdapter::appendText(const QString &text)
{
    if (!acceptsText(text)) {
        return false;
    }
    m_surface += text;
    refreshLookup();
    return true;
}

bool LibPinyinAdapter::removeLastInput()
{
    if (!hasComposition()) {
        return false;
    }
    m_surface.chop(1);
    refreshLookup();
    return true;
}

bool LibPinyinAdapter::hasComposition() const
{
    return !m_surface.isEmpty();
}

QString LibPinyinAdapter::preeditText() const
{
    return m_surface;
}

QStringList LibPinyinAdapter::candidates() const
{
    return m_candidates;
}

QString LibPinyinAdapter::takeSurface()
{
    QString text = m_surface;
    reset();
    return text;
}

void LibPinyinAdapter::reset()
{
    m_lookupCandidates.clear();
    if (m_instance) {
        pinyin_reset(m_instance);
    }
    m_surface.clear();
    m_candidates.clear();
}

QString LibPinyinAdapter::takeCommitText(int index)
{
    QString commitText = m_candidates.value(index, m_surface);

    if (commitText.isEmpty()) {
        return {};
    }

    QString remaining;
    if (auto *candidate = m_lookupCandidates.value(index)) {
        int offset = pinyin_choose_candidate(m_instance, 0, candidate);
        QByteArray surfaceUtf8 = m_surface.toUtf8();
        if (offset <= 0 || offset > surfaceUtf8.size()) {
            return {};
        }
        remaining = QString::fromUtf8(surfaceUtf8.mid(offset));
        while (remaining.startsWith(QLatin1Char('\''))) {
            remaining.remove(0, 1);
        }
    }

    m_surface = remaining;
    refreshLookup();
    return commitText;
}

bool LibPinyinAdapter::acceptsText(const QString &text)
{
    return text.size() == 1 && (text.at(0).isLetter() || text == QStringLiteral("'"));
}

void LibPinyinAdapter::refreshLookup()
{
    m_lookupCandidates.clear();
    if (m_surface.isEmpty()) {
        reset();
        return;
    }

    if (!rebuildLookup()) {
        m_candidates = {m_surface};
        return;
    }
}

bool LibPinyinAdapter::rebuildLookup()
{
    pinyin_context_t *context = sharedPinyinContext();
    if (!context) {
        return false;
    }

    if (m_instance) {
        pinyin_reset(m_instance);
    } else {
        m_instance = pinyin_alloc_instance(context);
        if (!m_instance) {
            return false;
        }
    }

    const QByteArray surfaceUtf8 = m_surface.toUtf8();
    pinyin_parse_more_full_pinyins(m_instance, surfaceUtf8.toLower().constData());
    const qsizetype parsedLength = qsizetype(pinyin_get_parsed_input_length(m_instance));
    const QString unparsedSuffix = QString::fromUtf8(surfaceUtf8.mid(parsedLength)); // Hold suffix if we are doing partial match (ex. 你hao)
    pinyin_guess_sentence(m_instance);
    if (!pinyin_guess_candidates(m_instance, 0, SORT_BY_PHRASE_LENGTH_AND_FREQUENCY)) {
        return false;
    }

    guint candidateCount = 0;
    if (!pinyin_get_n_candidate(m_instance, &candidateCount)) {
        return false;
    }

    QStringList candidateList;
    guint maxCandidates = qMin(candidateCount, CANDIDATES_LIMIT);
    for (guint i = 0; i < maxCandidates; ++i) {
        lookup_candidate_t *candidate = nullptr;
        const gchar *candidateUtf8 = nullptr;
        lookup_candidate_type_t type;
        if (!pinyin_get_candidate(m_instance, i, &candidate) || !candidate || !pinyin_get_candidate_string(m_instance, candidate, &candidateUtf8)
            || !candidateUtf8 || !pinyin_get_candidate_type(m_instance, candidate, &type)) {
            continue;
        }

        QString candidateText = QString::fromUtf8(candidateUtf8);
        if (type == NBEST_MATCH_CANDIDATE) {
            candidateText += unparsedSuffix;
            candidate = nullptr;
        }
        if (!candidateText.isEmpty()) {
            candidateList.append(candidateText);
            m_lookupCandidates.append(candidate);
        }
    }

    if (!candidateList.contains(m_surface)) {
        candidateList.append(m_surface);
        m_lookupCandidates.append(nullptr);
    }
    m_candidates = candidateList;
    return true;
}
