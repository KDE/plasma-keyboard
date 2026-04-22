/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "anthytextcomposer.h"

#include "../inputengine.h"
#include "logging.h"

#include <QByteArray>
#include <QHash>

#include <anthy/anthy.h>
#include <anthy/input.h>
#include <iconv.h>

using namespace Qt::StringLiterals;

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

    // Anthy exposes all text as EUC-JP
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

static QString preeditTextForContext(anthy_input_context *context)
{
    struct anthy_input_preedit *preedit = anthy_input_get_preedit(context);
    if (!preedit) {
        return {};
    }

    QString text = textFromSegments(preedit->segment);
    anthy_input_free_preedit(preedit);
    return text;
}

static QString cycledKanaChunk(const QString &chunk)
{
    static const QHash<QString, QString> replacements = [] {
        QHash<QString, QString> result;
        auto addCycle = [&result](const QStringList &cycle) {
            for (qsizetype index = 0; index < cycle.size(); ++index) {
                result.insert(cycle.at(index), cycle.at((index + 1) % cycle.size()));
            }
        };

        // Dakuten ゛
        addCycle({u"ka"_s, u"ga"_s});
        addCycle({u"ki"_s, u"gi"_s});
        addCycle({u"ku"_s, u"gu"_s});
        addCycle({u"ke"_s, u"ge"_s});
        addCycle({u"ko"_s, u"go"_s});
        addCycle({u"sa"_s, u"za"_s});
        addCycle({u"shi"_s, u"ji"_s});
        addCycle({u"su"_s, u"zu"_s});
        addCycle({u"se"_s, u"ze"_s});
        addCycle({u"so"_s, u"zo"_s});
        addCycle({u"ta"_s, u"da"_s});
        addCycle({u"chi"_s, u"di"_s});
        addCycle({u"te"_s, u"de"_s});
        addCycle({u"to"_s, u"do"_s});

        // Small 小 then dakuten ゛
        addCycle({u"tsu"_s, u"xtsu"_s, u"du"_s});

        // Dakuten ゛ then handakuten ゜
        addCycle({u"ha"_s, u"ba"_s, u"pa"_s});
        addCycle({u"hi"_s, u"bi"_s, u"pi"_s});
        addCycle({u"fu"_s, u"bu"_s, u"pu"_s});
        addCycle({u"he"_s, u"be"_s, u"pe"_s});
        addCycle({u"ho"_s, u"bo"_s, u"po"_s});

        // Small 小 and big 大
        addCycle({u"a"_s, u"xa"_s});
        addCycle({u"i"_s, u"xi"_s});
        addCycle({u"u"_s, u"xu"_s});
        addCycle({u"e"_s, u"xe"_s});
        addCycle({u"o"_s, u"xo"_s});
        addCycle({u"ya"_s, u"xya"_s});
        addCycle({u"yu"_s, u"xyu"_s});
        addCycle({u"yo"_s, u"xyo"_s});
        addCycle({u"wa"_s, u"xwa"_s});
        return result;
    }();
    return replacements.value(chunk);
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

static QString anthySegmentCandidate(anthy_context_t context, int segmentIndex, int candidateIndex)
{
    int length = anthy_get_segment(context, segmentIndex, candidateIndex, nullptr, 0);
    if (length <= 0) {
        return {};
    }

    QByteArray candidate(length + 1, Qt::Uninitialized);
    if (anthy_get_segment(context, segmentIndex, candidateIndex, candidate.data(), candidate.size()) < 0) {
        return {};
    }
    return decodeAnthyString(candidate.constData());
}

static QString anthyCombinedCandidate(anthy_context_t context, int segmentCount, int variedSegment, int candidateIndex)
{
    QString candidate;
    for (int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
        int segmentCandidateIndex = segmentIndex == variedSegment ? candidateIndex : 0;
        QString segmentCandidate = anthySegmentCandidate(context, segmentIndex, segmentCandidateIndex);
        if (segmentCandidate.isEmpty()) {
            return {};
        }
        candidate += segmentCandidate;
    }
    return candidate;
}

static QString anthyCombinedSpecialCandidate(anthy_context_t context, int segmentCount, int candidateIndex)
{
    QString candidate;
    for (int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
        QString segmentCandidate = anthySegmentCandidate(context, segmentIndex, candidateIndex);
        if (segmentCandidate.isEmpty()) {
            return {};
        }
        candidate += segmentCandidate;
    }
    return candidate;
}

static QStringList previewCandidatesForChunks(const QStringList &chunks, const QString &inputMode)
{
    if (chunks.isEmpty() || !ensureAnthyInitialized()) {
        return {};
    }

    // Use a separate context so preview conversion cannot change live composition
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
    anthy_context_t conversionContext = anthy_input_get_anthy_context(previewContext);
    anthy_conv_stat conversionStat;
    if (conversionContext && anthy_get_stat(conversionContext, &conversionStat) == 0 && conversionStat.nr_segment > 0) {
        auto appendCandidate = [&candidateList](const QString &candidate) {
            if (!candidate.trimmed().isEmpty() && !candidate.contains(u"〓"_s) && !candidateList.contains(candidate)) {
                candidateList.append(candidate);
            }
        };

        // Put whole-reading conversions before segmented alternatives
        appendCandidate(anthyCombinedSpecialCandidate(conversionContext, conversionStat.nr_segment, 0));
        appendCandidate(anthyCombinedSpecialCandidate(conversionContext, conversionStat.nr_segment, NTH_KATAKANA_CANDIDATE));
        appendCandidate(anthyCombinedSpecialCandidate(conversionContext, conversionStat.nr_segment, NTH_HIRAGANA_CANDIDATE));

        QList<int> candidateCounts;
        int maximumCandidateCount = 0;
        for (int segmentIndex = 0; segmentIndex < conversionStat.nr_segment; ++segmentIndex) {
            anthy_segment_stat segmentStat;
            if (anthy_get_segment_stat(conversionContext, segmentIndex, &segmentStat) != 0) {
                candidateCounts.append(0);
            } else {
                candidateCounts.append(segmentStat.nr_candidate);
                maximumCandidateCount = std::max(maximumCandidateCount, segmentStat.nr_candidate);
            }
        }

        // Interleave segments so one segment cannot consume the candidate limit
        for (int candidateIndex = 0; candidateIndex < maximumCandidateCount && candidateList.size() < 32; ++candidateIndex) {
            for (int segmentIndex = 0; segmentIndex < conversionStat.nr_segment && candidateList.size() < 32; ++segmentIndex) {
                if (candidateIndex >= candidateCounts.at(segmentIndex)) {
                    continue;
                }
                appendCandidate(anthyCombinedCandidate(conversionContext, conversionStat.nr_segment, segmentIndex, candidateIndex));
            }
        }
    }

    anthy_input_free_context(previewContext);
    anthy_input_free_config(previewConfig);
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

bool AnthyTextComposer::cycleKanaModifier()
{
    if (m_inputChunks.isEmpty()) {
        return false;
    }

    QString replacement = cycledKanaChunk(m_inputChunks.constLast());
    if (replacement.isEmpty()) {
        return false;
    }

    m_inputChunks.last() = replacement;
    updateComposition();
    return true;
}

bool AnthyTextComposer::autoCapitalizationSupported() const
{
    return false;
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
        return cycleCandidate();
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
        reset();
        return true;
    default:
        break;
    }

    const QString inputChunk = anthyChunkForText(text);
    if (inputChunk.isEmpty()) {
        if (hasComposition()) {
            const bool committed = commitComposition();
            return engine->handleKeyCommit(key, text) || committed;
        }
        return engine->handleKeyCommit(key, text);
    }
    m_inputChunks.append(inputChunk);
    updateComposition();
    return true;
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
        anthy_input_free_config(m_config);
        m_config = nullptr;
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

    // Anthy cannot replace an arbitrary input chunk
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

    const QString preeditText = preeditTextForContext(m_context);

    QStringList candidateList;
    if (!m_inputChunks.isEmpty() && !preeditText.isEmpty()) {
        candidateList = previewCandidates();
    }
    if (!m_selectedCandidate.isEmpty() && !candidateList.contains(m_selectedCandidate)) {
        m_selectedCandidate.clear();
    }
    if (!m_selectedCandidate.isEmpty()) {
        // Keep the selected candidate visible at the front
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

void AnthyTextComposer::updateComposition()
{
    m_selectedCandidate.clear();
    rebuildContext();
    refreshState();
}

QStringList AnthyTextComposer::previewCandidates() const
{
    return previewCandidatesForChunks(m_inputChunks, inputMode());
}

bool AnthyTextComposer::cycleCandidate()
{
    QStringList previewList = previewCandidates();
    if (previewList.isEmpty()) {
        return true;
    }

    int currentIndex = previewList.indexOf(m_selectedCandidate);
    m_selectedCandidate = previewList.at((currentIndex + 1) % previewList.size());
    refreshState();
    return true;
}

bool AnthyTextComposer::commitComposition()
{
    if (!m_context) {
        return false;
    }

    QString committedText = m_selectedCandidate;
    if (committedText.isEmpty()) {
        committedText = preeditTextForContext(m_context);
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

QString AnthyTextComposer::anthyChunkForText(const QString &text) const
{
    if (text.isEmpty()) {
        return {};
    }

    for (const QChar &character : text) {
        if (character.unicode() > 0x7f || (!character.isLetter() && character != QLatin1Char('\'') && character != QLatin1Char('-'))) {
            return {};
        }
    }
    return text.toLower();
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
    if (replacementChunk.isEmpty()) {
        return false;
    }
    m_inputChunks.last() = replacementChunk;
    updateComposition();
    return true;
}
