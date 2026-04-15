/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "libchewingadapter.h"

#include "config-plasma-keyboard.h"

#include <chewing/chewing.h>

static QString chewingString(const char *text)
{
    return text ? QString::fromUtf8(text) : QString();
}

static ChewingContext *createChewingContext()
{
    QString systemDir = QString::fromUtf8(PLASMA_KEYBOARD_LIBCHEWING_DATADIR);

    ChewingContext *context = chewing_new2(systemDir.toUtf8().constData(), ":memory:", nullptr, nullptr);
    if (!context) {
        return nullptr;
    }

    chewing_set_autoLearn(context, AUTOLEARN_DISABLED);
    chewing_set_KBType(context, KB_DEFAULT);
    chewing_set_ChiEngMode(context, CHINESE_MODE);
    chewing_set_ShapeMode(context, HALFSHAPE_MODE);
    chewing_set_easySymbolInput(context, 0);
    chewing_set_spaceAsSelection(context, 0);
    chewing_set_candPerPage(context, 10);
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

static QByteArray strokesForBopomofo(QStringView bopomofo)
{
    QByteArray strokes;
    for (QChar ch : bopomofo) {
        if (int stroke = defaultStrokeForChar(ch)) {
            strokes.append(char(stroke));
        }
    }
    return strokes;
}

static int strokeForText(const QString &text)
{
    if (text.size() != 1) {
        return 0;
    }

    return defaultStrokeForChar(text.at(0));
}

void LibChewingAdapter::ContextDeleter::operator()(ChewingContext *context) const
{
    chewing_delete(context);
}

bool LibChewingAdapter::initialize()
{
    if (!m_context) {
        m_context.reset(createChewingContext());
    }
    return bool(m_context);
}

bool LibChewingAdapter::appendText(const QString &text)
{
    int stroke = strokeForText(text);
    if (stroke == 0 || !initialize()) {
        return false;
    }
    chewing_handle_Default(m_context.get(), stroke);
    collectPendingText();
    refreshState();
    return true;
}

bool LibChewingAdapter::applyFirstTone()
{
    if (!hasPendingBopomofo()) {
        return false;
    }

    const QString originalBopomofo = chewingString(chewing_bopomofo_String_static(m_context.get()));
    const QString originalBuffer = chewingString(chewing_buffer_String_static(m_context.get()));
    chewing_handle_Space(m_context.get());

    // Restore if tone converesion fails
    if (!chewing_bopomofo_Check(m_context.get()) && !chewing_commit_Check(m_context.get())
        && chewingString(chewing_buffer_String_static(m_context.get())) == originalBuffer) {
        for (char stroke : strokesForBopomofo(originalBopomofo)) {
            chewing_handle_Default(m_context.get(), stroke);
        }
    }

    collectPendingText();
    refreshState();
    return true;
}

bool LibChewingAdapter::removeLastInput()
{
    if (!hasComposition()) {
        return false;
    }
    chewing_handle_Backspace(m_context.get());
    collectPendingText();
    refreshState();
    return true;
}

bool LibChewingAdapter::selectCandidate(int index)
{
    if (!initialize() || index < 0 || index >= m_candidates.size()) {
        return false;
    }

    if (m_candidatesUsePreviewContext) {
        chewing_handle_Space(m_context.get());
        collectPendingText();
    }
    chewing_handle_Down(m_context.get());
    if (chewing_cand_choose_by_index(m_context.get(), index) != 0) {
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

    QString text =
        takeCommitText() + chewingString(chewing_buffer_String_static(m_context.get())) + chewingString(chewing_bopomofo_String_static(m_context.get()));
    reset();
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
        chewing_Reset(m_context.get());
    }
    m_candidatesUsePreviewContext = false;
    m_preeditPrefix.clear();
    m_preeditText.clear();
    m_candidates.clear();
    m_commitText.clear();
}

void LibChewingAdapter::refreshState()
{
    m_candidatesUsePreviewContext = false;
    m_candidates.clear();

    const QString buffer = m_context ? chewingString(chewing_buffer_String_static(m_context.get())) : QString();
    const QString bopomofo = m_context ? chewingString(chewing_bopomofo_String_static(m_context.get())) : QString();
    m_preeditText = buffer + bopomofo;
    m_preeditPrefix = bopomofo.isEmpty() ? QString() : buffer;

    if (!m_preeditText.isEmpty()) {
        updateCandidates();
    }
}

bool LibChewingAdapter::hasComposition() const
{
    return m_context && (chewing_buffer_Check(m_context.get()) || chewing_bopomofo_Check(m_context.get()));
}

bool LibChewingAdapter::hasPendingBopomofo() const
{
    return m_context && chewing_bopomofo_Check(m_context.get());
}

void LibChewingAdapter::collectPendingText()
{
    if (!m_context || !chewing_commit_Check(m_context.get())) {
        return;
    }

    const QString text = chewingString(chewing_commit_String_static(m_context.get()));
    chewing_ack(m_context.get());
    m_commitText += text;
}

void LibChewingAdapter::updateCandidates()
{
    // Syllables without a tone have no candidates yet! so look them up in preview context
    if (chewing_bopomofo_Check(m_context.get())) {
        m_candidatesUsePreviewContext = updatePreviewCandidates();
        return;
    }

    if (chewing_handle_Down(m_context.get()) != 0) {
        return;
    }
    collectCandidates(m_context.get());
    chewing_cand_close(m_context.get());
}

void LibChewingAdapter::collectCandidates(ChewingContext *context)
{
    int totalChoices = chewing_cand_TotalChoice(context);
    m_candidates.reserve(totalChoices);

    for (int i = 0; i < totalChoices; ++i) {
        m_candidates.append(chewingString(chewing_cand_string_by_index_static(context, i)));
    }
}

bool LibChewingAdapter::updatePreviewCandidates()
{
    if (!m_previewContext) {
        m_previewContext.reset(createChewingContext());
        if (!m_previewContext) {
            return false;
        }
    }
    chewing_Reset(m_previewContext.get());

    for (char stroke : strokesForBopomofo(chewingString(chewing_bopomofo_String_static(m_context.get())))) {
        chewing_handle_Default(m_previewContext.get(), stroke);
    }
    chewing_handle_Space(m_previewContext.get());
    if (chewing_handle_Down(m_previewContext.get()) != 0) {
        return false;
    }

    collectCandidates(m_previewContext.get());
    chewing_cand_close(m_previewContext.get());
    return !m_candidates.isEmpty();
}
