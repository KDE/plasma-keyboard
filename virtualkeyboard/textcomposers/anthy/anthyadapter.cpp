/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "anthyadapter.h"

#include "logging.h"

#include <QByteArray>
#include <QHash>

#include <algorithm>
#include <utility>

#include <anthy/anthy.h>
#include <anthy/input.h>
#include <iconv.h>

using namespace Qt::StringLiterals;

static constexpr int MAXIMUM_CONVERSIONS = 30;

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

static anthy_context *createUtf8Context()
{
    anthy_context *context = anthy_create_context();
    if (context) {
        anthy_context_set_encoding(context, ANTHY_UTF8_ENCODING);
    }
    return context;
}

// anthy outputs in EUC_JP not UTF-8
static QString fromEucJp(const char *text)
{
    static const iconv_t converter = iconv_open("UTF-8", "EUC-JP");
    if (converter == iconv_t(-1)) {
        return {};
    }

    QByteArray input(text);
    QByteArray output(input.size() * 2, Qt::Uninitialized);
    char *inputData = input.data();
    char *outputData = output.data();
    size_t inputBytesLeft = size_t(input.size());
    size_t outputBytesLeft = size_t(output.size());
    if (iconv(converter, &inputData, &inputBytesLeft, &outputData, &outputBytesLeft) == size_t(-1)) {
        iconv(converter, nullptr, nullptr, nullptr, nullptr);
        return {};
    }
    return QString::fromUtf8(output.constData(), output.size() - qsizetype(outputBytesLeft));
}

// Given no buffer, anthy returns the length of the text instead
static QString segmentText(anthy_context *context, int segment, int candidate)
{
    QByteArray buffer(anthy_get_segment(context, segment, candidate, nullptr, 0) + 1, Qt::Uninitialized);
    if (buffer.size() <= 1 || anthy_get_segment(context, segment, candidate, buffer.data(), int(buffer.size())) < 0) {
        return {};
    }
    return QString::fromUtf8(buffer.constData());
}

void AnthyAdapter::AnthyDeleter::operator()(anthy_context *context) const
{
    anthy_release_context(context);
}

void AnthyAdapter::AnthyDeleter::operator()(anthy_input_context *context) const
{
    anthy_input_free_context(context);
}

void AnthyAdapter::AnthyDeleter::operator()(anthy_input_config *config) const
{
    anthy_input_free_config(config);
}

struct AnthySpelling {
    QString kana;
    QString pending;
};

static AnthySpelling typeCharacter(anthy_input_context *context, QChar character)
{
    const char key[] = {char(character.toLower().unicode()), '\0'};
    anthy_input_str(context, key);

    AnthySpelling spelling;
    anthy_input_preedit *preedit = anthy_input_get_preedit(context);
    if (!preedit) {
        return spelling;
    }
    for (anthy_input_segment *segment = preedit->segment; segment; segment = segment->next) {
        // The cursor is a segment with no text
        if (segment->str) {
            (segment->flag & ANTHY_INPUT_SF_PENDING ? spelling.pending : spelling.kana) += fromEucJp(segment->str);
        }
    }
    anthy_input_free_preedit(preedit);
    return spelling;
}

static bool isLoneN(const QString &romaji)
{
    return romaji.compare(u"n"_s, Qt::CaseInsensitive) == 0;
}

static QString toKatakana(QString text)
{
    for (QChar &character : text) {
        const char16_t code = character.unicode();
        if ((code >= u'ぁ' && code <= u'ゖ') || code == u'ゝ' || code == u'ゞ') {
            // Hiragana and katakana are in parallel (0x60 difference)
            character = QChar(code + 0x60);
        }
    }
    return text;
}

// The romaji that the kana modifier key (小゛゜) switches a key press to (ex. "ka" to "ga")
static QString nextKanaModifier(const QString &romaji)
{
    static const QHash<QString, QString> nextModifiers = [] {
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
        addCycle({u"u"_s, u"xu"_s, u"vu"_s});
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
        addCycle({u"e"_s, u"xe"_s});
        addCycle({u"o"_s, u"xo"_s});
        addCycle({u"ya"_s, u"xya"_s});
        addCycle({u"yu"_s, u"xyu"_s});
        addCycle({u"yo"_s, u"xyo"_s});
        addCycle({u"wa"_s, u"xwa"_s});

        return result;
    }();
    return nextModifiers.value(romaji.toLower());
}

// Setup

AnthyAdapter::AnthyAdapter() = default;
AnthyAdapter::~AnthyAdapter() = default;

bool AnthyAdapter::acceptsText(const QString &text)
{
    // Also accept JP form of punctuation (’ー〜、。)
    static constexpr QStringView punctuation = u"'-~,.";
    return !text.isEmpty() && std::ranges::all_of(text, [](QChar character) {
        return (character.unicode() < 0x80 && character.isLetter()) || punctuation.contains(character);
    });
}

bool AnthyAdapter::initialize()
{
    if (m_conversionContext) {
        return true;
    }
    if (!ensureAnthyInitialized()) {
        return false;
    }

    m_inputConfig.reset(anthy_input_create_config());
    if (m_inputConfig) {
        m_inputContext.reset(anthy_input_create_context(m_inputConfig.get()));
        m_probeContext.reset(anthy_input_create_context(m_inputConfig.get()));
    }
    m_conversionContext.reset(createUtf8Context());

    if (!m_inputContext || !m_probeContext || !m_conversionContext) {
        qCWarning(PlasmaKeyboard) << "AnthyAdapter: failed to create anthy contexts";
        m_conversionContext.reset();
        m_probeContext.reset();
        m_inputContext.reset();
        m_inputConfig.reset();
        return false;
    }
    anthy_input_map_select(m_inputContext.get(), ANTHY_INPUT_MAP_HIRAGANA);
    anthy_input_map_select(m_probeContext.get(), ANTHY_INPUT_MAP_HIRAGANA);
    return true;
}

void AnthyAdapter::setKatakana(bool katakana)
{
    m_katakana = katakana;
    updateCandidates();
}

bool AnthyAdapter::appendText(const QString &text)
{
    if (!acceptsText(text) || !initialize()) {
        return false;
    }
    m_keyPresses.append(text);
    updateComposition();
    return true;
}

bool AnthyAdapter::removeLastInput()
{
    if (m_keyPresses.isEmpty()) {
        return false;
    }
    m_keyPresses.removeLast();
    updateComposition();
    return true;
}

bool AnthyAdapter::replaceLastInput(const QString &text)
{
    if (m_keyPresses.isEmpty() || !acceptsText(text)) {
        return false;
    }
    m_keyPresses.last() = text;
    updateComposition();
    return true;
}

bool AnthyAdapter::cycleKanaModifier()
{
    if (m_keyPresses.isEmpty()) {
        return false;
    }
    QString modified = nextKanaModifier(m_keyPresses.constLast());
    return !modified.isEmpty() && replaceLastInput(modified);
}

void AnthyAdapter::updateComposition()
{
    m_reading = spellOut(m_keyPresses.join(QString()));
    updateCandidates();
}

// Convert romaji into kana
AnthyAdapter::Reading AnthyAdapter::spellOut(const QString &romaji) const
{
    anthy_input_quit(m_inputContext.get());

    Reading reading;
    qsizetype anthyKanaLength = 0;
    for (QChar character : romaji) {
        // "n'" spells ん when it would otherwise join a following vowel (ex. "kan'i" for かんい)
        if (character == u'\'' && isLoneN(reading.pendingRomaji)) {
            character = u'n';
        }

        const AnthySpelling typed = typeCharacter(m_inputContext.get(), character);
        const QString newKana = typed.kana.mid(anthyKanaLength);
        anthyKanaLength = typed.kana.size();

        // Restore dropped characters that anthy removes when it can't spell the kana
        anthy_input_quit(m_probeContext.get());
        const AnthySpelling alone = typeCharacter(m_probeContext.get(), character);
        const bool droppedPending = !reading.pendingRomaji.isEmpty() && newKana == alone.kana && typed.pending == alone.pending;
        const bool droppedCharacter = alone.kana.isEmpty() && alone.pending.isEmpty() && typed.pending.isEmpty();

        if (droppedPending) {
            reading.kana += std::exchange(reading.pendingRomaji, QString());
        }
        reading.kana += newKana;
        if (droppedCharacter) {
            reading.kana += character;
        }

        // Anthy only shows pending romaji in lowercase, keep it as it was typed
        reading.pendingRomaji = QString(reading.pendingRomaji + character).right(typed.pending.size());
    }

    // Anthy spells "vu" with a separate dakuten
    reading.kana.replace(u"う゛"_s, u"ゔ"_s);
    return reading;
}

// HACK: Exception for n, to avoid anthy also interpreting it as ん
QString AnthyAdapter::readingForConversion() const
{
    return isLoneN(m_reading.pendingRomaji) ? m_reading.kana + u"ん"_s : m_reading.kana;
}

// Romaji that can't be converted yet
// HACK: Exception for n, to avoid anthy also interpreting it as ん
QString AnthyAdapter::leftoverRomaji() const
{
    return isLoneN(m_reading.pendingRomaji) ? QString() : m_reading.pendingRomaji;
}

QString AnthyAdapter::toInputScript(const QString &hiragana) const
{
    return m_katakana ? toKatakana(hiragana) : hiragana;
}

void AnthyAdapter::updateCandidates()
{
    m_candidates.clear();
    m_selectedCandidate = -1;

    const QString reading = readingForConversion();
    if (reading.isEmpty()) {
        return;
    }

    m_candidates = conversionCandidates(reading);

    // HACK: remove empty and 〓 character that anthy gives for some reason
    m_candidates.removeIf([](const QString &candidate) {
        return candidate.isEmpty() || candidate.contains(u'〓');
    });
    m_candidates.removeDuplicates();
}

QStringList AnthyAdapter::conversionCandidates(const QString &reading) const
{
    anthy_context *context = m_conversionContext.get();
    anthy_conv_stat stat;
    if (anthy_set_string(context, reading.toUtf8().constData()) != 0 || anthy_get_stat(context, &stat) != 0 || stat.nr_segment <= 0) {
        return {};
    }

    // Fetch segments lists from anthy (list of predictions per segment)
    QList<QStringList> segments;
    for (int segment = 0; segment < stat.nr_segment; ++segment) {
        anthy_segment_stat segmentStat{};
        anthy_get_segment_stat(context, segment, &segmentStat);
        QStringList texts;
        for (int candidate = 0; candidate < std::min(segmentStat.nr_candidate, MAXIMUM_CONVERSIONS); ++candidate) {
            texts.append(segmentText(context, segment, candidate));
        }
        if (texts.isEmpty()) {
            return {};
        }
        segments.append(texts);
    }

    // Join best predictions for each segment from anthy
    // Start with first/best prediction from each segment, then work our way down in having all combinations
    // TODO: better algorithm for candidate selection?
    QStringList best;
    for (const QStringList &texts : std::as_const(segments)) {
        best.append(texts.first());
    }
    QStringList result{best.join(QString())};
    for (int rank = 1; rank < MAXIMUM_CONVERSIONS && result.size() < MAXIMUM_CONVERSIONS; ++rank) {
        for (int segment = 0; segment < segments.size(); ++segment) {
            if (rank < segments.at(segment).size()) {
                QStringList alternative = best;
                alternative[segment] = segments.at(segment).at(rank);
                result.append(alternative.join(QString()));
            }
        }
    }

    // Also add the reading directly at the end of the suggestions (romaji, katakana)
    result.append(toInputScript(reading));
    result.append(m_katakana ? reading : toKatakana(reading));
    return result;
}

bool AnthyAdapter::isConverting() const
{
    return m_selectedCandidate >= 0;
}

bool AnthyAdapter::selectNextCandidate()
{
    if (m_candidates.isEmpty()) {
        return false;
    }
    m_selectedCandidate = (m_selectedCandidate + 1) % m_candidates.size();
    return true;
}

bool AnthyAdapter::selectCandidate(int index)
{
    if (index < 0 || index >= m_candidates.size()) {
        return false;
    }
    m_selectedCandidate = index;
    return true;
}

void AnthyAdapter::cancelConversion()
{
    m_selectedCandidate = -1;
}

QString AnthyAdapter::takeCommitText()
{
    QString text;
    if (isConverting()) {
        text = m_candidates.at(m_selectedCandidate) + leftoverRomaji();
    } else {
        text = toInputScript(readingForConversion()) + leftoverRomaji();
    }
    reset();
    return text;
}

void AnthyAdapter::reset()
{
    m_keyPresses.clear();
    m_reading = {};
    m_candidates.clear();
    m_selectedCandidate = -1;
}

// State

bool AnthyAdapter::hasComposition() const
{
    return !m_keyPresses.isEmpty();
}

QString AnthyAdapter::preeditText() const
{
    if (isConverting()) {
        return m_candidates.at(m_selectedCandidate) + leftoverRomaji();
    }
    return toInputScript(m_reading.kana) + m_reading.pendingRomaji;
}

QStringList AnthyAdapter::candidates() const
{
    return m_candidates;
}
