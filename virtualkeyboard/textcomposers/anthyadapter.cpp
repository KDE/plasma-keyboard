/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "anthyadapter.h"

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
            qCWarning(PlasmaKeyboard) << "AnthyAdapter: failed to initialize anthy";
            return false;
        }
        if (anthy_input_init() != 0) {
            qCWarning(PlasmaKeyboard) << "AnthyAdapter: failed to initialize anthy input layer";
            return false;
        }
        return true;
    }();

    return initialized;
}

static QByteArray convertEncoding(QByteArray input, const char *to, const char *from)
{
    iconv_t converter = iconv_open(to, from);
    if (converter == iconv_t(-1)) {
        return input;
    }
    QByteArray output(input.size() * 4 + 16, Qt::Uninitialized);
    char *inputPtr = input.data();
    char *outputPtr = output.data();
    size_t inputBytesLeft = size_t(input.size());
    size_t outputBytesLeft = size_t(output.size());
    size_t result = iconv(converter, &inputPtr, &inputBytesLeft, &outputPtr, &outputBytesLeft);
    iconv_close(converter);
    if (result == size_t(-1)) {
        return input;
    }
    output.truncate(output.size() - qsizetype(outputBytesLeft));
    return output;
}

static QString decodeAnthyString(const char *text)
{
    return QString::fromUtf8(convertEncoding(QByteArray(text), "UTF-8", "EUC-JP"));
}

static QString cycledKanaChunk(const QString &chunk)
{
    static const QHash<QString, QString> replacements = [] {
        QHash<QString, QString> result;
        auto addCycle = [&result](const QStringList &cycle) {
            for (int index = 0; index < cycle.size(); ++index) {
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
    return replacements.value(chunk.toLower());
}

static int anthyMapForInputMode(const QString &inputMode)
{
    if (inputMode == u"katakana"_s) {
        return ANTHY_INPUT_MAP_KATAKANA;
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

static QString combinedCandidate(anthy_context_t context, int segmentCount, int candidateIndex, int variedSegment = -1)
{
    QString candidate;
    for (int segment = 0; segment < segmentCount; ++segment) {
        int index = variedSegment < 0 || segment == variedSegment ? candidateIndex : 0;
        QString text = anthySegmentCandidate(context, segment, index);
        if (text.isEmpty()) {
            return {};
        }
        candidate += text;
    }
    return candidate;
}

struct Preedit {
    QString converted;
    QString pending;
};

static Preedit preeditForContext(anthy_input_context *context)
{
    auto *preedit = anthy_input_get_preedit(context);
    if (!preedit) {
        return {};
    }

    Preedit parts;
    for (auto *segment = preedit->segment; segment; segment = segment->next) {
        if (segment->flag & ANTHY_INPUT_SF_PENDING) {
            parts.pending += decodeAnthyString(segment->str);
        } else {
            parts.converted += decodeAnthyString(segment->str);
        }
    }
    anthy_input_free_preedit(preedit);
    return parts;
}

static QStringList candidatesForReading(anthy_context_t context, const QString &reading)
{
    if (reading.isEmpty() || anthy_set_string(context, convertEncoding(reading.toLower().toUtf8(), "EUC-JP", "UTF-8").constData()) != 0) {
        return {};
    }
    anthy_conv_stat stat;
    if (anthy_get_stat(context, &stat) != 0 || stat.nr_segment <= 0) {
        return {};
    }

    QStringList candidates;
    auto append = [&candidates](const QString &candidate) {
        // HACK: 〓 for some reason comes out of the candidates for anthy
        if (!candidate.trimmed().isEmpty() && !candidate.contains(u"〓"_s) && !candidates.contains(candidate)) {
            candidates.append(candidate);
        }
    };
    for (int index : {0, NTH_KATAKANA_CANDIDATE, NTH_HIRAGANA_CANDIDATE}) {
        append(combinedCandidate(context, stat.nr_segment, index));
    }

    QList<int> counts;
    int maximumCount = 0;
    for (int segment = 0; segment < stat.nr_segment; ++segment) {
        anthy_segment_stat segmentStat;
        int count = anthy_get_segment_stat(context, segment, &segmentStat) == 0 ? segmentStat.nr_candidate : 0;
        counts.append(count);
        maximumCount = std::max(maximumCount, count);
    }
    // HACK: Interleave candidate alternatives
    // TODO make the suggestions better and more relevant
    for (int index = 0; index < maximumCount && candidates.size() < 32; ++index) {
        for (int segment = 0; segment < stat.nr_segment && candidates.size() < 32; ++segment) {
            if (index < counts.at(segment)) {
                append(combinedCandidate(context, stat.nr_segment, index, segment));
            }
        }
    }
    return candidates;
}

AnthyAdapter::AnthyAdapter() = default;

AnthyAdapter::~AnthyAdapter()
{
    if (m_conversionContext) {
        anthy_release_context(m_conversionContext);
    }
    if (m_context) {
        anthy_input_free_context(m_context);
    }
    if (m_config) {
        anthy_input_free_config(m_config);
    }
}

bool AnthyAdapter::initialize()
{
    if (m_context) {
        return true;
    }
    if (!ensureAnthyInitialized()) {
        return false;
    }

    m_config = anthy_input_create_config();
    if (!m_config) {
        qCWarning(PlasmaKeyboard) << "AnthyAdapter: failed to create anthy config";
        return false;
    }

    m_context = anthy_input_create_context(m_config);
    if (!m_context) {
        qCWarning(PlasmaKeyboard) << "AnthyAdapter: failed to create anthy context";
        anthy_input_free_config(m_config);
        m_config = nullptr;
        return false;
    }

    m_conversionContext = anthy_create_context();
    applyInputMode();
    return true;
}

void AnthyAdapter::setInputMode(const QString &inputMode)
{
    reset();
    m_inputMode = inputMode;
    applyInputMode();
}

void AnthyAdapter::applyInputMode()
{
    if (m_context) {
        anthy_input_map_select(m_context, anthyMapForInputMode(m_inputMode));
    }
}

bool AnthyAdapter::acceptsText(const QString &text)
{
    if (text.isEmpty()) {
        return false;
    }
    for (QChar character : text) {
        if (character.unicode() > 0x7f || (!character.isLetter() && character != QLatin1Char('\'') && character != QLatin1Char('-'))) {
            return false;
        }
    }
    return true;
}

bool AnthyAdapter::appendText(const QString &text)
{
    if (!acceptsText(text) || !initialize()) {
        return false;
    }
    m_inputChunks.append(text);
    for (QChar character : text) {
        appendCharacter(character);
    }
    updateCandidates();
    return true;
}

bool AnthyAdapter::removeLastInput()
{
    if (m_inputChunks.isEmpty()) {
        return false;
    }
    m_inputChunks.removeLast();
    rebuildContext();
    return true;
}

bool AnthyAdapter::replaceLastInput(const QString &text)
{
    if (m_inputChunks.isEmpty() || !acceptsText(text)) {
        return false;
    }
    m_inputChunks.last() = text;
    rebuildContext();
    return true;
}

bool AnthyAdapter::cycleKanaModifier()
{
    if (m_inputChunks.isEmpty()) {
        return false;
    }
    QString replacement = cycledKanaChunk(m_inputChunks.constLast().toLower());
    if (replacement.isEmpty()) {
        return false;
    }
    return replaceLastInput(replacement);
}

void AnthyAdapter::updateCandidates()
{
    m_selectedCandidate.clear();
    QString reading = m_convertedText;
    // A pending n becomes the syllabic nasal when requesting conversion
    if (m_pendingText.toLower() == u"n"_s) {
        reading += m_inputMode == u"katakana"_s ? u"ン"_s : u"ん"_s;
    }
    m_candidates = m_conversionContext ? candidatesForReading(m_conversionContext, reading) : QStringList();
}

void AnthyAdapter::appendCharacter(QChar character)
{
    QString input = m_pendingText + character;
    anthy_input_str(m_context, QString(character.toLower()).toUtf8().constData());
    Preedit next = preeditForContext(m_context);
    if (!m_pendingText.isEmpty() && m_convertedText == next.converted && !next.pending.startsWith(m_pendingText.toLower())) {
        // Preserve a discarded romaji prefix as literal text, in its original case
        anthy_input_map_select(m_context, ANTHY_INPUT_MAP_ALPHABET);
        anthy_input_str(m_context, m_pendingText.toUtf8().constData());
        applyInputMode();
        anthy_input_str(m_context, QString(character.toLower()).toUtf8().constData());
        next = preeditForContext(m_context);
    }
    m_convertedText = next.converted;
    m_pendingText = input.right(next.pending.size());
}

void AnthyAdapter::rebuildContext()
{
    anthy_input_quit(m_context);
    applyInputMode();
    m_convertedText.clear();
    m_pendingText.clear();
    for (QChar character : m_inputChunks.join(QString())) {
        appendCharacter(character);
    }
    updateCandidates();
}

bool AnthyAdapter::hasComposition() const
{
    return !m_inputChunks.isEmpty();
}

QString AnthyAdapter::preeditText() const
{
    return m_selectedCandidate.isEmpty() ? m_convertedText + m_pendingText : m_selectedCandidate;
}

QStringList AnthyAdapter::candidates() const
{
    QStringList result = m_candidates;
    int selectedIndex = result.indexOf(m_selectedCandidate);
    if (selectedIndex > 0) {
        result.move(selectedIndex, 0);
    }
    return result;
}

bool AnthyAdapter::selectCandidate(int index)
{
    QStringList visibleCandidates = candidates();
    if (index < 0 || index >= visibleCandidates.size()) {
        return false;
    }
    m_selectedCandidate = visibleCandidates.at(index);
    return true;
}

void AnthyAdapter::cycleCandidate()
{
    if (m_candidates.isEmpty()) {
        return;
    }
    int currentIndex = m_candidates.indexOf(m_selectedCandidate);
    m_selectedCandidate = m_candidates.at((currentIndex + 1) % m_candidates.size());
}

QString AnthyAdapter::takeCommitText()
{
    QString text = preeditText();
    reset();
    return text;
}

void AnthyAdapter::reset()
{
    if (m_context) {
        anthy_input_quit(m_context);
        applyInputMode();
    }
    m_inputChunks.clear();
    m_convertedText.clear();
    m_pendingText.clear();
    m_selectedCandidate.clear();
    m_candidates.clear();
}
