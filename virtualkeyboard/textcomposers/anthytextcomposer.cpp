/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "anthytextcomposer.h"

#include "../inputengine.h"
#include "logging.h"

#include <QByteArray>

#include <anthy/anthy.h>
#include <anthy/input.h>
#include <iconv.h>

using namespace Qt::StringLiterals;

static bool isKanaModifierText(const QString &text)
{
    return text == QString(QChar(0x3099)) || text == QString(QChar(0x309A)) || text == u"small"_s;
}

static bool ensureAnthyInitialized()
{
    static const bool initialized = [] {
        if (anthy_init() != 0) {
            qCWarning(PlasmaKeyboard) << "AnthyTextComposer: failed to initialize anthy";
            return false;
        }
        if (anthy_input_init() != 0) {
            qCWarning(PlasmaKeyboard) << "AnthyTextComposer: failed to initialize anthy input layer";
            return false;
        }
        return true;
    }();

    return initialized;
}

static QString decodeAnthyString(const char *text)
{
    if (!text || !*text) {
        return {};
    }

    const QByteArray bytes(text);
    iconv_t converter = iconv_open("UTF-8", "EUC-JP");
    if (converter == iconv_t(-1)) {
        return QString::fromUtf8(bytes);
    }

    QByteArray input = bytes;
    QByteArray output(bytes.size() * 4 + 16, Qt::Uninitialized);
    char *inputPtr = input.data();
    char *outputPtr = output.data();
    size_t inputBytesLeft = size_t(input.size());
    size_t outputBytesLeft = size_t(output.size());
    const size_t result = iconv(converter, &inputPtr, &inputBytesLeft, &outputPtr, &outputBytesLeft);
    iconv_close(converter);

    if (result == size_t(-1)) {
        return QString::fromUtf8(bytes);
    }

    output.truncate(output.size() - qsizetype(outputBytesLeft));
    return QString::fromUtf8(output);
}

static QString textFromSegments(struct anthy_input_segment *segment)
{
    QString text;
    for (struct anthy_input_segment *current = segment; current; current = current->next) {
        text += decodeAnthyString(current->str);
    }
    return text;
}

static QString dakutenChunk(const QString &chunk)
{
    if (chunk == u"ka"_s)
        return u"ga"_s;
    if (chunk == u"ki"_s)
        return u"gi"_s;
    if (chunk == u"ku"_s)
        return u"gu"_s;
    if (chunk == u"ke"_s)
        return u"ge"_s;
    if (chunk == u"ko"_s)
        return u"go"_s;
    if (chunk == u"sa"_s)
        return u"za"_s;
    if (chunk == u"shi"_s)
        return u"ji"_s;
    if (chunk == u"su"_s)
        return u"zu"_s;
    if (chunk == u"se"_s)
        return u"ze"_s;
    if (chunk == u"so"_s)
        return u"zo"_s;
    if (chunk == u"ta"_s)
        return u"da"_s;
    if (chunk == u"chi"_s)
        return u"ji"_s;
    if (chunk == u"tsu"_s)
        return u"zu"_s;
    if (chunk == u"te"_s)
        return u"de"_s;
    if (chunk == u"to"_s)
        return u"do"_s;
    if (chunk == u"ha"_s)
        return u"ba"_s;
    if (chunk == u"hi"_s)
        return u"bi"_s;
    if (chunk == u"fu"_s)
        return u"bu"_s;
    if (chunk == u"he"_s)
        return u"be"_s;
    if (chunk == u"ho"_s)
        return u"bo"_s;
    return {};
}

static QString handakutenChunk(const QString &chunk)
{
    if (chunk == u"ha"_s)
        return u"pa"_s;
    if (chunk == u"hi"_s)
        return u"pi"_s;
    if (chunk == u"fu"_s)
        return u"pu"_s;
    if (chunk == u"he"_s)
        return u"pe"_s;
    if (chunk == u"ho"_s)
        return u"po"_s;
    return {};
}

static QString smallKanaChunk(const QString &chunk)
{
    if (chunk == u"a"_s)
        return u"xa"_s;
    if (chunk == u"i"_s)
        return u"xi"_s;
    if (chunk == u"u"_s)
        return u"xu"_s;
    if (chunk == u"e"_s)
        return u"xe"_s;
    if (chunk == u"o"_s)
        return u"xo"_s;
    if (chunk == u"tsu"_s)
        return u"xtsu"_s;
    if (chunk == u"ya"_s)
        return u"xya"_s;
    if (chunk == u"yu"_s)
        return u"xyu"_s;
    if (chunk == u"yo"_s)
        return u"xyo"_s;
    if (chunk == u"wa"_s)
        return u"xwa"_s;
    return {};
}

static int anthyMapForInputMode(const QString &inputMode)
{
    if (inputMode == u"katakana"_s) {
        return ANTHY_INPUT_MAP_KATAKANA;
    }
    if (inputMode == u"latin"_s) {
        return ANTHY_INPUT_MAP_ALPHABET;
    }
    return ANTHY_INPUT_MAP_HIRAGANA;
}

static QStringList previewCandidatesForChunks(const QStringList &chunks, const QString &inputMode)
{
    if (chunks.isEmpty() || !ensureAnthyInitialized()) {
        return {};
    }

    anthy_input_config *previewConfig = anthy_input_create_config();
    if (!previewConfig) {
        return {};
    }

    anthy_input_context *previewContext = anthy_input_create_context(previewConfig);
    if (!previewContext) {
        anthy_input_free_config(previewConfig);
        return {};
    }

    anthy_input_map_select(previewContext, anthyMapForInputMode(inputMode));
    for (const QString &chunk : chunks) {
        anthy_input_str(previewContext, chunk.toUtf8().constData());
    }
    anthy_input_space(previewContext);

    QStringList candidateList;
    anthy_input_preedit *preedit = anthy_input_get_preedit(previewContext);
    int candidateCount = 0;
    for (anthy_input_segment *segment = preedit ? preedit->segment : nullptr; segment; segment = segment->next) {
        candidateCount = std::max(candidateCount, segment->nr_cand);
    }
    if (preedit) {
        anthy_input_free_preedit(preedit);
    }

    for (int i = 0; i < candidateCount && i < 32; ++i) {
        anthy_input_segment *candidate = anthy_input_get_candidate(previewContext, i);
        if (!candidate) {
            break;
        }
        const QString candidateText = decodeAnthyString(candidate->str);
        if (!candidateText.trimmed().isEmpty() && candidateText != u"〓"_s) {
            candidateList.append(candidateText);
        }
        anthy_input_free_segment(candidate);
    }

    anthy_input_free_context(previewContext);
    anthy_input_free_config(previewConfig);
    candidateList.removeDuplicates();
    return candidateList;
}

AnthyTextComposer::AnthyTextComposer(QObject *parent)
    : AbstractTextComposer(parent)
{
}

AnthyTextComposer::~AnthyTextComposer()
{
    if (m_context) {
        anthy_input_free_context(m_context);
    }
    if (m_config) {
        anthy_input_free_config(m_config);
    }
}

QString AnthyTextComposer::inputMode() const
{
    return m_inputMode;
}

void AnthyTextComposer::setInputMode(const QString &inputMode)
{
    if (m_inputMode == inputMode || inputMode.isEmpty()) {
        return;
    }

    reset();
    m_inputMode = inputMode;
    applyInputMode(inputMode);
    Q_EMIT inputModeChanged();
}

bool AnthyTextComposer::setTextCase(TextCase textCase)
{
    Q_UNUSED(textCase)
    return true;
}

bool AnthyTextComposer::keyEvent(Qt::Key key, const QString &text)
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    if (inputMode() == u"latin"_s) {
        return engine->handleKeyCommit(key, text);
    }

    ensureContext();
    if (!m_context) {
        return engine->handleKeyCommit(key, text);
    }

    switch (key) {
    case Qt::Key_Backspace:
        if (!hasComposition()) {
            return engine->handleKeyCommit(key, text);
        }
        m_selectedCandidate.clear();
        if (!m_inputChunks.isEmpty()) {
            m_inputChunks.removeLast();
            rebuildContext();
        } else {
            anthy_input_erase_prev(m_context);
        }
        refreshState();
        return true;
    case Qt::Key_Space:
        if (!hasComposition()) {
            return engine->handleKeyCommit(key, text);
        }
        {
            const QStringList previewList = previewCandidates();
            if (previewList.isEmpty()) {
                return true;
            }
            int nextIndex = 0;
            if (!m_selectedCandidate.isEmpty()) {
                const int currentIndex = previewList.indexOf(m_selectedCandidate);
                if (currentIndex >= 0) {
                    nextIndex = (currentIndex + 1) % previewList.size();
                }
            }
            m_selectedCandidate = previewList.at(nextIndex);
        }
        refreshState();
        return true;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (!hasComposition()) {
            return engine->handleKeyCommit(key, text);
        }
        return commitComposition();
    case Qt::Key_Escape:
        if (!hasComposition()) {
            return false;
        }
        anthy_input_quit(m_context);
        m_inputChunks.clear();
        refreshState();
        return true;
    default:
        break;
    }

    if (applyDakutenMark(text)) {
        m_selectedCandidate.clear();
        rebuildContext();
        refreshState();
        return true;
    }
    if (isKanaModifierText(text)) {
        return true;
    }

    const QString inputChunk = anthyChunkForText(text);
    if (!inputChunk.isEmpty()) {
        m_selectedCandidate.clear();
        m_inputChunks.append(inputChunk);
        rebuildContext();
        refreshState();
        return true;
    }

    if (hasComposition()) {
        const bool committed = commitComposition();
        return engine->handleKeyCommit(key, text) || committed;
    }

    return engine->handleKeyCommit(key, text);
}

bool AnthyTextComposer::selectCandidate(int index)
{
    if (index < 0 || index >= candidates().size()) {
        return false;
    }

    m_selectedCandidate = candidates().at(index);
    return commitComposition();
}

bool AnthyTextComposer::showsPreeditBubble() const
{
    return true;
}

void AnthyTextComposer::reset()
{
    if (m_context) {
        anthy_input_quit(m_context);
    }
    m_inputChunks.clear();
    m_selectedCandidate.clear();
    clearComposition();
}

void AnthyTextComposer::update()
{
    refreshState();
}

void AnthyTextComposer::ensureContext()
{
    if (m_context || !ensureAnthyInitialized()) {
        return;
    }

    m_config = anthy_input_create_config();
    if (!m_config) {
        qCWarning(PlasmaKeyboard) << "AnthyTextComposer: failed to create anthy config";
        return;
    }

    m_context = anthy_input_create_context(m_config);
    if (!m_context) {
        qCWarning(PlasmaKeyboard) << "AnthyTextComposer: failed to create anthy context";
        return;
    }

    applyInputMode(inputMode());
}

void AnthyTextComposer::applyInputMode(const QString &inputMode)
{
    if (!m_context) {
        return;
    }
    anthy_input_map_select(m_context, anthyMapForInputMode(inputMode));
}

void AnthyTextComposer::rebuildContext()
{
    if (!m_context) {
        return;
    }

    anthy_input_quit(m_context);
    applyInputMode(inputMode());

    for (const QString &chunk : std::as_const(m_inputChunks)) {
        anthy_input_str(m_context, chunk.toUtf8().constData());
    }
}

void AnthyTextComposer::refreshState()
{
    if (!m_context) {
        clearComposition();
        return;
    }

    struct anthy_input_preedit *preedit = anthy_input_get_preedit(m_context);
    if (!preedit) {
        clearComposition();
        return;
    }

    const QString preeditText = textFromSegments(preedit->segment);
    anthy_input_free_preedit(preedit);

    QStringList candidateList;
    if (!m_inputChunks.isEmpty() && !preeditText.isEmpty()) {
        candidateList = previewCandidates();
    }
    candidateList.removeDuplicates();
    if (!m_selectedCandidate.isEmpty() && !candidateList.contains(m_selectedCandidate)) {
        m_selectedCandidate.clear();
    }
    if (!m_selectedCandidate.isEmpty()) {
        const int selectedIndex = candidateList.indexOf(m_selectedCandidate);
        if (selectedIndex > 0) {
            candidateList.move(selectedIndex, 0);
        }
    }

    const QString visiblePreeditText = m_selectedCandidate.isEmpty() ? preeditText : m_selectedCandidate;
    setPreeditPrefix(QString());
    setPreeditText(visiblePreeditText);
    setCandidates(candidateList);
}

QStringList AnthyTextComposer::previewCandidates() const
{
    return previewCandidatesForChunks(m_inputChunks, inputMode());
}

bool AnthyTextComposer::commitComposition()
{
    if (!m_context) {
        return false;
    }

    QString committedText = m_selectedCandidate;
    if (committedText.isEmpty()) {
        struct anthy_input_preedit *preedit = anthy_input_get_preedit(m_context);
        committedText = preedit ? textFromSegments(preedit->segment) : QString();
        if (preedit) {
            anthy_input_free_preedit(preedit);
        }
    }

    anthy_input_quit(m_context);
    applyInputMode(inputMode());

    m_inputChunks.clear();
    m_selectedCandidate.clear();

    if (!committedText.isEmpty()) {
        if (auto *engine = inputEngine()) {
            engine->commit(committedText);
        }
        return true;
    }

    clearComposition();
    return false;
}

bool AnthyTextComposer::hasComposition() const
{
    return !m_inputChunks.isEmpty() || !m_selectedCandidate.isEmpty() || !preeditText().isEmpty() || !candidates().isEmpty();
}

bool AnthyTextComposer::applyDakutenMark(const QString &text)
{
    if (m_inputChunks.isEmpty() || text.isEmpty()) {
        return false;
    }

    QString replacement;
    if (text == QString(QChar(0x3099))) {
        replacement = dakutenChunk(m_inputChunks.constLast());
    } else if (text == QString(QChar(0x309A))) {
        replacement = handakutenChunk(m_inputChunks.constLast());
    } else if (text == u"small"_s) {
        replacement = smallKanaChunk(m_inputChunks.constLast());
    } else {
        return false;
    }

    if (replacement.isEmpty()) {
        return false;
    }

    m_inputChunks.last() = replacement;
    return true;
}

bool AnthyTextComposer::isRomajiInput(const QString &text) const
{
    if (text.isEmpty()) {
        return false;
    }

    for (const QChar &ch : text) {
        if (!ch.isLetter() && ch != QLatin1Char('\'')) {
            return false;
        }
    }
    return true;
}

QString AnthyTextComposer::anthyChunkForText(const QString &text) const
{
    if (isRomajiInput(text)) {
        return text.toLower();
    }
    return {};
}

bool AnthyTextComposer::replaceLastInput(const QString &text)
{
    if (inputMode() == u"latin"_s) {
        if (text.isEmpty()) {
            return false;
        }

        if (auto *engine = inputEngine()) {
            engine->commit(text, -1, 1);
            return true;
        }
        return false;
    }

    if (m_inputChunks.isEmpty()) {
        return false;
    }

    const QString replacementChunk = anthyChunkForText(text);
    if (!replacementChunk.isEmpty()) {
        m_inputChunks.last() = replacementChunk;
    } else if (!applyDakutenMark(text)) {
        return false;
    }

    m_selectedCandidate.clear();
    rebuildContext();
    refreshState();
    return true;
}
