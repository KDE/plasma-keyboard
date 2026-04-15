/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "pinyintextcomposer.h"

#include "inputengine.h"

#include <QLocale>

PinyinTextComposer::PinyinTextComposer(QObject *parent)
    : AbstractTextComposer(parent)
{
}

PinyinTextComposer::~PinyinTextComposer() = default;

QString PinyinTextComposer::inputMode() const
{
    return m_inputMode;
}

void PinyinTextComposer::setInputMode(const QString &inputMode)
{
    if (m_inputMode == inputMode || inputMode.isEmpty()) {
        return;
    }

    commitCurrentSelection();
    reset();
    m_inputMode = inputMode;
    Q_EMIT inputModeChanged();
}

bool PinyinTextComposer::autoCapitalizationSupported() const
{
    return false;
}

bool PinyinTextComposer::setTextCase(TextCase textCase)
{
    m_textCase = textCase;
    return true;
}

bool PinyinTextComposer::keyEvent(Qt::Key key, const QString &text)
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    if (inputMode() == QStringLiteral("latin")) {
        return engine->handleKeyCommit(key, transformedText(text));
    }

    if (m_pinyin.hasComposition()) {
        switch (key) {
        case Qt::Key_Backspace:
            m_pinyin.removeLastInput();
            refreshState();
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
    }

    if (m_pinyin.appendText(transformedText(text))) {
        refreshState();
        return true;
    }

    bool committed = m_pinyin.hasComposition() && commitCurrentSelection();
    return engine->handleKeyCommit(key, text) || committed;
}

bool PinyinTextComposer::selectCandidate(int index)
{
    const QStringList currentCandidates = m_pinyin.candidates();
    if (index < 0 || index >= currentCandidates.size()) {
        return false;
    }

    return commitCurrentSelection(index);
}

bool PinyinTextComposer::showsPreeditBubble() const
{
    return true;
}

void PinyinTextComposer::reset()
{
    m_pinyin.reset();
    clearComposition();
}

void PinyinTextComposer::refreshState()
{
    setPreeditPrefix(QString());
    setPreeditText(m_pinyin.preeditText());
    setCandidates(m_pinyin.candidates());
}

bool PinyinTextComposer::commitCurrentSelection(int index)
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    QString text = m_pinyin.takeCommitText(index);
    if (text.isEmpty()) {
        return false;
    }
    engine->commit(text);
    refreshState();
    return true;
}

bool PinyinTextComposer::commitSurface()
{
    auto *engine = inputEngine();
    if (!engine || !m_pinyin.hasComposition()) {
        return false;
    }

    engine->commit(m_pinyin.takeSurface());
    refreshState();
    return true;
}

QString PinyinTextComposer::transformedText(const QString &text) const
{
    if (m_textCase != TextCase::Upper || text.size() != 1) {
        return text;
    }

    return QLocale(inputEngine() ? inputEngine()->locale() : QString()).toUpper(text);
}
