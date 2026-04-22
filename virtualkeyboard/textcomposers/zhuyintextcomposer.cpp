/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "zhuyintextcomposer.h"

#include "../inputengine.h"
#include "config-plasma-keyboard.h"

#include <QDir>
#include <QStandardPaths>

#include <chewing/chewing.h>

using namespace Qt::StringLiterals;

static QString chewingString(const char *text)
{
    return text ? QString::fromUtf8(text) : QString();
}

static QString chewingUserDir()
{
    const QString userDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/libchewing");
    QDir().mkpath(userDir);
    return userDir;
}

static ChewingContext *createChewingContext()
{
    const QString systemDir = QString::fromUtf8(PLASMA_KEYBOARD_LIBCHEWING_DATADIR);
    const QString userDir = chewingUserDir();
    return chewing_new2(systemDir.toUtf8().constData(), userDir.toUtf8().constData(), nullptr, nullptr);
}

// libchewing takes in ascii keyboard keys corresponding to bopomofo, so we need this mapping function
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

ZhuyinTextComposer::ZhuyinTextComposer(QObject *parent)
    : AbstractTextComposer(parent)
{
}

ZhuyinTextComposer::~ZhuyinTextComposer()
{
    if (m_context) {
        chewing_delete(m_context);
    }
}

QString ZhuyinTextComposer::inputMode() const
{
    return m_inputMode;
}

void ZhuyinTextComposer::setInputMode(const QString &inputMode)
{
    if (inputMode.isEmpty() || inputMode == m_inputMode) {
        return;
    }

    if (hasComposition()) {
        commitPreeditBuffer();
    }
    m_inputMode = inputMode;
    Q_EMIT inputModeChanged();
}

bool ZhuyinTextComposer::setTextCase(TextCase textCase)
{
    m_textCase = textCase;
    return true;
}

bool ZhuyinTextComposer::keyEvent(Qt::Key key, const QString &text)
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    if (inputMode() == u"latin"_s) {
        return engine->handleKeyCommit(key, transformedText(text));
    }

    ensureContext();
    if (!m_context) {
        return engine->handleKeyCommit(key, text);
    }

    switch (key) {
    case Qt::Key_Backspace:
        if (hasComposition()) {
            if (!m_strokeBuffer.isEmpty()) {
                m_strokeBuffer.chop(1);
            }
            chewing_handle_Backspace(m_context);
            commitPendingText();
            refreshState();
            return true;
        }
        return engine->handleKeyCommit(key, text);
    case Qt::Key_Escape:
        if (!hasComposition()) {
            return false;
        }
        chewing_Reset(m_context);
        refreshState();
        return true;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        if (hasComposition()) {
            return commitPreeditBuffer();
        }
        return engine->handleKeyCommit(key, text);
    default:
        break;
    }

    const int stroke = strokeForText(text);
    if (stroke != 0) {
        m_strokeBuffer.append(char(stroke));
        chewing_handle_Default(m_context, stroke);
        commitPendingText();
        refreshState();
        return true;
    }

    if (hasComposition()) {
        const bool committed = commitPreeditBuffer();
        return engine->handleKeyCommit(key, text) || committed;
    }

    return engine->handleKeyCommit(key, text);
}

bool ZhuyinTextComposer::selectCandidate(int index)
{
    ensureContext();
    if (!m_context) {
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
    return commitPreeditBuffer();
}

bool ZhuyinTextComposer::showsPreeditBubble() const
{
    return true;
}

void ZhuyinTextComposer::reset()
{
    if (m_context) {
        chewing_Reset(m_context);
    }
    m_strokeBuffer.clear();
    m_candidatesUsePreviewContext = false;
    clearComposition();
}

void ZhuyinTextComposer::update()
{
    refreshState();
}

void ZhuyinTextComposer::ensureContext()
{
    if (m_context) {
        return;
    }

    m_context = createChewingContext();
    if (!m_context) {
        return;
    }

    chewing_set_KBType(m_context, chewingLayoutId());
    chewing_set_ChiEngMode(m_context, CHINESE_MODE);
    chewing_set_ShapeMode(m_context, HALFSHAPE_MODE);
    chewing_set_easySymbolInput(m_context, 0);
    chewing_set_spaceAsSelection(m_context, 0);
    chewing_set_candPerPage(m_context, 10);
}

void ZhuyinTextComposer::refreshState()
{
    if (!m_context) {
        clearComposition();
        return;
    }

    const QString buffer = chewingString(chewing_buffer_String_static(m_context));
    const QString bopomofo = chewingString(chewing_bopomofo_String_static(m_context));

    if (buffer.isEmpty() && bopomofo.isEmpty()) {
        clearComposition();
        return;
    }

    const QStringList candidateList = currentCandidates();
    setCandidates(candidateList);

    if (!buffer.isEmpty() && !bopomofo.isEmpty()) {
        setPreeditPrefix(buffer);
        setPreeditText(buffer + bopomofo);
        return;
    }

    setPreeditPrefix(QString());
    setPreeditText(!bopomofo.isEmpty() ? bopomofo : buffer);
}

bool ZhuyinTextComposer::hasComposition() const
{
    return m_context && (chewing_buffer_Check(m_context) || chewing_bopomofo_Check(m_context));
}

bool ZhuyinTextComposer::commitPendingText()
{
    auto *engine = inputEngine();
    if (!engine || !m_context || !chewing_commit_Check(m_context)) {
        return false;
    }

    const QString text = chewingString(chewing_commit_String_static(m_context));
    chewing_ack(m_context);
    if (text.isEmpty()) {
        return false;
    }

    engine->commit(text);
    m_strokeBuffer.clear();
    return true;
}

bool ZhuyinTextComposer::commitPreeditBuffer()
{
    if (!m_context) {
        return false;
    }

    chewing_commit_preedit_buf(m_context);
    const bool committed = commitPendingText();
    refreshState();
    return committed;
}

QStringList ZhuyinTextComposer::currentCandidates()
{
    if (!m_context) {
        return {};
    }

    m_candidatesUsePreviewContext = false;
    if (chewing_handle_Down(m_context) != 0) {
        return previewCandidatesForCurrentStrokeBuffer();
    }

    QStringList candidateList;
    const int totalChoices = chewing_cand_TotalChoice(m_context);
    candidateList.reserve(totalChoices);
    for (int i = 0; i < totalChoices; ++i) {
        const QString candidate = chewingString(chewing_cand_string_by_index_static(m_context, i));
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

QStringList ZhuyinTextComposer::previewCandidatesForCurrentStrokeBuffer() const
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
    const int totalChoices = chewing_cand_TotalChoice(previewContext.get());
    candidateList.reserve(totalChoices);
    for (int i = 0; i < totalChoices; ++i) {
        const QString candidate = chewingString(chewing_cand_string_by_index_static(previewContext.get(), i));
        if (!candidate.isEmpty()) {
            candidateList.append(candidate);
        }
    }
    candidateList.removeDuplicates();
    if (!candidateList.isEmpty()) {
        const_cast<ZhuyinTextComposer *>(this)->m_candidatesUsePreviewContext = true;
    }
    return candidateList;
}

ChewingContext *ZhuyinTextComposer::createPreviewContext() const
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

    for (const char stroke : m_strokeBuffer) {
        chewing_handle_Default(context, stroke);
    }

    return context;
}

int ZhuyinTextComposer::chewingLayoutId() const
{
    return KB_DEFAULT;
}

int ZhuyinTextComposer::strokeForText(const QString &text) const
{
    if (text.size() != 1) {
        return 0;
    }

    return defaultStrokeForChar(text.at(0));
}

QString ZhuyinTextComposer::transformedText(const QString &text) const
{
    if (m_textCase != TextCase::Upper || text.size() != 1) {
        return text;
    }

    return QLocale(inputEngine() ? inputEngine()->locale() : QString()).toUpper(text);
}
