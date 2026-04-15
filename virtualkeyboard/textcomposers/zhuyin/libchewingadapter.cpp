/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "libchewingadapter.h"

#include "config-plasma-keyboard.h"

#include <memory>

#include <chewing/chewing.h>

static QString chewingString(const char *text)
{
    return text ? QString::fromUtf8(text) : QString();
}

static ChewingContext *createChewingContext()
{
    QString systemDir = QString::fromUtf8(PLASMA_KEYBOARD_LIBCHEWING_DATADIR);

    ChewingContext *context = chewing_new2(systemDir.toUtf8().constData(), ":memory:", nullptr, nullptr);
    if (context) {
        chewing_set_autoLearn(context, AUTOLEARN_DISABLED);
    }
    return context;
}

// libchewing takes in ascii keyboard keys corresponding to bopomofo so we need this mapping function
static int defaultStrokeForChar(QChar ch)
{
    switch (ch.unicode()) {
    case 0x3105: // ㄅ
        return '1';
    case 0x3109: // ㄉ
        return '2';
    case 0x02c7: // ˇ
        return '3';
    case 0x02cb: // ˋ
        return '4';
    case 0x3113: // ㄓ
        return '5';
    case 0x02ca: // ˊ
        return '6';
    case 0x02d9: // ˙
        return '7';
    case 0x311a: // ㄚ
        return '8';
    case 0x311e: // ㄞ
        return '9';
    case 0x3122: // ㄢ
        return '0';
    case 0x3106: // ㄆ
        return 'q';
    case 0x310a: // ㄊ
        return 'w';
    case 0x310d: // ㄍ
        return 'e';
    case 0x3110: // ㄐ
        return 'r';
    case 0x3114: // ㄔ
        return 't';
    case 0x3117: // ㄗ
        return 'y';
    case 0x3127: // ㄧ
        return 'u';
    case 0x311b: // ㄛ
        return 'i';
    case 0x311f: // ㄟ
        return 'o';
    case 0x3123: // ㄣ
        return 'p';
    case 0x3107: // ㄇ
        return 'a';
    case 0x310b: // ㄋ
        return 's';
    case 0x310e: // ㄎ
        return 'd';
    case 0x3111: // ㄑ
        return 'f';
    case 0x3115: // ㄕ
        return 'g';
    case 0x3118: // ㄘ
        return 'h';
    case 0x3128: // ㄨ
        return 'j';
    case 0x311c: // ㄜ
        return 'k';
    case 0x3120: // ㄠ
        return 'l';
    case 0x3108: // ㄈ
        return 'z';
    case 0x310c: // ㄌ
        return 'x';
    case 0x310f: // ㄏ
        return 'c';
    case 0x3112: // ㄒ
        return 'v';
    case 0x3116: // ㄖ
        return 'b';
    case 0x3119: // ㄙ
        return 'n';
    case 0x3129: // ㄩ
        return 'm';
    case 0x311d: // ㄝ
        return ',';
    case 0x3121: // ㄡ
        return '.';
    case 0x3125: // ㄥ
        return '/';
    case 0x3124: // ㄤ
        return ';';
    case 0x3126: // ㄦ
        return '-';
    default:
        return 0;
    }
}

LibChewingAdapter::~LibChewingAdapter()
{
    if (m_context) {
        chewing_delete(m_context);
    }
}

bool LibChewingAdapter::initialize()
{
    if (m_context) {
        return true;
    }

    m_context = createChewingContext();
    if (!m_context) {
        return false;
    }

    chewing_set_KBType(m_context, chewingLayoutId());
    chewing_set_ChiEngMode(m_context, CHINESE_MODE);
    chewing_set_ShapeMode(m_context, HALFSHAPE_MODE);
    chewing_set_easySymbolInput(m_context, 0);
    chewing_set_spaceAsSelection(m_context, 0);
    chewing_set_candPerPage(m_context, 10);
    return true;
}

bool LibChewingAdapter::appendText(const QString &text)
{
    int stroke = strokeForText(text);
    if (stroke == 0 || !initialize()) {
        return false;
    }
    m_strokeBuffer.append(char(stroke));
    chewing_handle_Default(m_context, stroke);
    collectPendingText();
    refreshState();
    return true;
}

bool LibChewingAdapter::removeLastInput()
{
    if (!hasComposition()) {
        return false;
    }
    if (!m_strokeBuffer.isEmpty()) {
        m_strokeBuffer.chop(1);
    }
    chewing_handle_Backspace(m_context);
    collectPendingText();
    refreshState();
    return true;
}

bool LibChewingAdapter::selectCandidate(int index)
{
    if (!initialize()) {
        return false;
    }

    if (m_candidatesUsePreviewContext) {
        chewing_handle_Space(m_context);
        chewing_handle_Down(m_context);
    } else if (chewing_handle_Down(m_context) != 0) {
        return false;
    }
    if (chewing_cand_choose_by_index(m_context, index) != 0) {
        refreshState();
        return false;
    }
    return true;
}

QString LibChewingAdapter::takeCommitText()
{
    QString text = m_commitText;
    m_commitText.clear();
    return text;
}

QString LibChewingAdapter::takePreeditText()
{
    if (!m_context) {
        return {};
    }

    QString originalPending = chewingString(chewing_bopomofo_String_static(m_context));
    QString originalBuffer = chewingString(chewing_buffer_String_static(m_context));
    if (!originalPending.isEmpty()) {
        chewing_handle_Space(m_context);
    }
    QString buffer = chewingString(chewing_buffer_String_static(m_context));
    QString pending = chewingString(chewing_bopomofo_String_static(m_context));
    // A failed first-tone conversion can clear the phonetics without adding a character.
    if (!originalPending.isEmpty() && pending.isEmpty() && buffer == originalBuffer && !chewing_commit_Check(m_context)) {
        pending = originalPending;
    }
    if (!pending.isEmpty()) {
        QString text = takeCommitText() + buffer + pending;
        reset();
        return text;
    }

    chewing_commit_preedit_buf(m_context);
    collectPendingText();
    QString text = takeCommitText();
    if (!text.isEmpty()) {
        reset();
    } else {
        refreshState();
    }
    return text;
}

QString LibChewingAdapter::preeditText() const
{
    return m_preeditText;
}

QString LibChewingAdapter::preeditPrefix() const
{
    return m_preeditPrefix;
}

QStringList LibChewingAdapter::candidates() const
{
    return m_candidates;
}

void LibChewingAdapter::reset()
{
    if (m_context) {
        chewing_Reset(m_context);
    }
    m_strokeBuffer.clear();
    m_candidatesUsePreviewContext = false;
    m_preeditPrefix.clear();
    m_preeditText.clear();
    m_candidates.clear();
    m_commitText.clear();
}

void LibChewingAdapter::refreshState()
{
    if (!m_context) {
        m_preeditPrefix.clear();
        m_preeditText.clear();
        m_candidates.clear();
        return;
    }

    QString buffer = chewingString(chewing_buffer_String_static(m_context));
    QString bopomofo = chewingString(chewing_bopomofo_String_static(m_context));

    if (buffer.isEmpty() && bopomofo.isEmpty()) {
        m_preeditPrefix.clear();
        m_preeditText.clear();
        m_candidates.clear();
        return;
    }

    QStringList candidateList = currentCandidates();
    m_candidates = candidateList;

    if (!buffer.isEmpty() && !bopomofo.isEmpty()) {
        m_preeditPrefix = buffer;
        m_preeditText = buffer + bopomofo;
        return;
    }

    m_preeditPrefix.clear();
    m_preeditText = !bopomofo.isEmpty() ? bopomofo : buffer;
}

bool LibChewingAdapter::hasComposition() const
{
    return m_context && (chewing_buffer_Check(m_context) || chewing_bopomofo_Check(m_context));
}

void LibChewingAdapter::collectPendingText()
{
    if (!m_context || !chewing_commit_Check(m_context)) {
        return;
    }

    const QString text = chewingString(chewing_commit_String_static(m_context));
    chewing_ack(m_context);
    if (text.isEmpty()) {
        return;
    }

    m_commitText += text;
    m_strokeBuffer.clear();
}

QStringList LibChewingAdapter::currentCandidates()
{
    if (!m_context) {
        return {};
    }

    m_candidatesUsePreviewContext = false;
    if (chewing_handle_Down(m_context) != 0) {
        return previewCandidatesForCurrentStrokeBuffer();
    }

    QStringList candidateList;
    int totalChoices = chewing_cand_TotalChoice(m_context);
    candidateList.reserve(totalChoices);
    for (int i = 0; i < totalChoices; ++i) {
        QString candidate = chewingString(chewing_cand_string_by_index_static(m_context, i));
        if (!candidate.isEmpty()) {
            candidateList.append(candidate);
        }
    }

    chewing_cand_close(m_context);
    candidateList.removeDuplicates();
    if (candidateList.isEmpty()) {
        return previewCandidatesForCurrentStrokeBuffer();
    }
    return candidateList;
}

QStringList LibChewingAdapter::previewCandidatesForCurrentStrokeBuffer()
{
    if (m_strokeBuffer.isEmpty()) {
        return {};
    }

    std::unique_ptr<ChewingContext, decltype(&chewing_delete)> previewContext(createPreviewContext(), &chewing_delete);
    if (!previewContext) {
        return {};
    }

    chewing_handle_Space(previewContext.get());
    if (chewing_handle_Down(previewContext.get()) != 0) {
        return {};
    }

    QStringList candidateList;
    int totalChoices = chewing_cand_TotalChoice(previewContext.get());
    candidateList.reserve(totalChoices);
    for (int i = 0; i < totalChoices; ++i) {
        QString candidate = chewingString(chewing_cand_string_by_index_static(previewContext.get(), i));
        if (!candidate.isEmpty()) {
            candidateList.append(candidate);
        }
    }
    candidateList.removeDuplicates();
    if (!candidateList.isEmpty()) {
        m_candidatesUsePreviewContext = true;
    }
    return candidateList;
}

ChewingContext *LibChewingAdapter::createPreviewContext() const
{
    ChewingContext *context = createChewingContext();
    if (!context) {
        return nullptr;
    }

    chewing_set_KBType(context, chewingLayoutId());
    chewing_set_ChiEngMode(context, CHINESE_MODE);
    chewing_set_ShapeMode(context, HALFSHAPE_MODE);
    chewing_set_easySymbolInput(context, 0);
    chewing_set_spaceAsSelection(context, 0);
    chewing_set_candPerPage(context, 10);

    for (char stroke : m_strokeBuffer) {
        chewing_handle_Default(context, stroke);
    }

    return context;
}

int LibChewingAdapter::chewingLayoutId() const
{
    return KB_DEFAULT;
}

int LibChewingAdapter::strokeForText(const QString &text) const
{
    if (text.size() != 1) {
        return 0;
    }

    return defaultStrokeForChar(text.at(0));
}
