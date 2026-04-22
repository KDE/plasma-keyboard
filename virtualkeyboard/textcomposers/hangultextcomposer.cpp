/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "hangultextcomposer.h"

#include "../inputengine.h"

#include <QLocale>
#include <hangul.h>

static QString stringFromUcs(const ucschar *text)
{
    if (!text) {
        return {};
    }

    QString result;
    for (int i = 0; text[i] != 0; ++i) {
        result.append(QChar(char32_t(text[i])));
    }
    return result;
}

// libhangul takes in ascii keyboard keys corresponding to hangul, so we need this mapping function
static int asciiForHangulJamo(QChar ch)
{
    switch (ch.unicode()) {
    case 0x3142: // ㅂ
        return 'q';
    case 0x3143: // ㅃ
        return 'Q';
    case 0x3148: // ㅈ
        return 'w';
    case 0x3149: // ㅉ
        return 'W';
    case 0x3137: // ㄷ
        return 'e';
    case 0x3138: // ㄸ
        return 'E';
    case 0x3131: // ㄱ
        return 'r';
    case 0x3132: // ㄲ
        return 'R';
    case 0x3145: // ㅅ
        return 't';
    case 0x3146: // ㅆ
        return 'T';
    case 0x315B: // ㅛ
        return 'y';
    case 0x3155: // ㅕ
        return 'u';
    case 0x3151: // ㅑ
        return 'i';
    case 0x3150: // ㅐ
        return 'o';
    case 0x3152: // ㅒ
        return 'O';
    case 0x3154: // ㅔ
        return 'p';
    case 0x3156: // ㅖ
        return 'P';
    case 0x3141: // ㅁ
        return 'a';
    case 0x3134: // ㄴ
        return 's';
    case 0x3147: // ㅇ
        return 'd';
    case 0x3139: // ㄹ
        return 'f';
    case 0x314E: // ㅎ
        return 'g';
    case 0x3157: // ㅗ
        return 'h';
    case 0x3153: // ㅓ
        return 'j';
    case 0x314F: // ㅏ
        return 'k';
    case 0x3163: // ㅣ
        return 'l';
    case 0x314B: // ㅋ
        return 'z';
    case 0x314C: // ㅌ
        return 'x';
    case 0x314A: // ㅊ
        return 'c';
    case 0x314D: // ㅍ
        return 'v';
    case 0x3160: // ㅠ
        return 'b';
    case 0x315C: // ㅜ
        return 'n';
    case 0x3161: // ㅡ
        return 'm';
    default:
        return 0;
    }
}

HangulTextComposer::HangulTextComposer(QObject *parent)
    : AbstractTextComposer(parent)
{
}

HangulTextComposer::~HangulTextComposer()
{
    if (m_context) {
        hangul_ic_delete(m_context);
    }
}

QString HangulTextComposer::inputMode() const
{
    return m_inputMode;
}

void HangulTextComposer::setInputMode(const QString &inputMode)
{
    if (inputMode.isEmpty() || inputMode == m_inputMode) {
        return;
    }

    flushComposition();
    m_inputMode = inputMode;
    Q_EMIT inputModeChanged();
}

bool HangulTextComposer::setTextCase(TextCase textCase)
{
    m_textCase = textCase;
    return true;
}

bool HangulTextComposer::keyEvent(Qt::Key key, const QString &text)
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    if (inputMode() == QStringLiteral("latin")) {
        flushComposition();
        return engine->handleKeyCommit(key, transformedText(text));
    }

    ensureContext();
    if (!m_context) {
        return engine->handleKeyCommit(key, text);
    }

    if (key == Qt::Key_Backspace) {
        if (hangul_ic_backspace(m_context)) {
            updatePreedit();
            return true;
        }
        return engine->handleKeyCommit(key, text);
    }

    if (key == Qt::Key_Escape) {
        if (hangul_ic_is_empty(m_context)) {
            return false;
        }
        hangul_ic_reset(m_context);
        updatePreedit();
        return true;
    }

    if (!text.isEmpty() && text.size() == 1) {
        const int ascii = asciiForHangulJamo(text.at(0));
        if (ascii != 0) {
            return processHangulKey(ascii, key, text);
        }
    }

    const bool flushed = flushComposition();
    return engine->handleKeyCommit(key, text) || flushed;
}

void HangulTextComposer::reset()
{
    if (m_context) {
        hangul_ic_reset(m_context);
    }
    clearComposition();
}

void HangulTextComposer::update()
{
    updatePreedit();
}

void HangulTextComposer::ensureContext()
{
    if (m_context) {
        return;
    }

    m_context = hangul_ic_new("2");
}

void HangulTextComposer::updatePreedit()
{
    if (!m_context) {
        clearComposition();
        return;
    }

    setPreeditPrefix(QString());
    setPreeditText(stringFromUcs(hangul_ic_get_preedit_string(m_context)));
    setCandidates({});
}

bool HangulTextComposer::processHangulKey(int ascii, Qt::Key fallbackKey, const QString &fallbackText)
{
    auto *engine = inputEngine();
    if (!engine || !m_context) {
        return false;
    }

    if (!hangul_ic_process(m_context, ascii)) {
        return engine->handleKeyCommit(fallbackKey, fallbackText);
    }

    const QString commitText = stringFromUcs(hangul_ic_get_commit_string(m_context));
    if (!commitText.isEmpty()) {
        engine->commit(commitText);
    }
    updatePreedit();
    return true;
}

bool HangulTextComposer::flushComposition()
{
    auto *engine = inputEngine();
    if (!engine || !m_context || hangul_ic_is_empty(m_context)) {
        return false;
    }

    const QString text = stringFromUcs(hangul_ic_flush(m_context));
    if (text.isEmpty()) {
        clearComposition();
        return false;
    }

    engine->commit(text);
    updatePreedit();
    return true;
}

QString HangulTextComposer::transformedText(const QString &text) const
{
    if (m_textCase != TextCase::Upper || text.size() != 1) {
        return text;
    }

    return QLocale(inputEngine() ? inputEngine()->locale() : QString()).toUpper(text);
}
