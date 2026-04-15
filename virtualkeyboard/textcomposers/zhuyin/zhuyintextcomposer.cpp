/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "zhuyintextcomposer.h"

#include "inputengine.h"

#include <QLocale>

using namespace Qt::StringLiterals;

ZhuyinTextComposer::ZhuyinTextComposer(QObject *parent)
    : AbstractTextComposer(parent)
{
}

ZhuyinTextComposer::~ZhuyinTextComposer() = default;

QString ZhuyinTextComposer::inputMode() const
{
    return m_inputMode;
}

void ZhuyinTextComposer::setInputMode(const QString &inputMode)
{
    if (inputMode.isEmpty() || inputMode == m_inputMode) {
        return;
    }

    if (m_chewing.hasComposition()) {
        commitPreeditBuffer();
    }
    reset();
    m_inputMode = inputMode;
    Q_EMIT inputModeChanged();
}

bool ZhuyinTextComposer::autoCapitalizationSupported() const
{
    return false;
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

    if (!m_chewing.initialize()) {
        return engine->handleKeyCommit(key, text);
    }

    switch (key) {
    case Qt::Key_Backspace:
        if (m_chewing.hasComposition()) {
            m_chewing.removeLastInput();
            commitPendingText();
            refreshState();
            return true;
        }
        return engine->handleKeyCommit(key, text);
    case Qt::Key_Escape:
        if (!m_chewing.hasComposition()) {
            return false;
        }
        reset();
        return true;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        if (m_chewing.hasComposition()) {
            return commitPreeditBuffer();
        }
        return engine->handleKeyCommit(key, text);
    default:
        break;
    }

    if (m_chewing.appendText(text)) {
        commitPendingText();
        refreshState();
        return true;
    }

    if (m_chewing.hasComposition()) {
        const bool committed = commitPreeditBuffer();
        return engine->handleKeyCommit(key, text) || committed;
    }

    return engine->handleKeyCommit(key, text);
}

bool ZhuyinTextComposer::selectCandidate(int index)
{
    if (!m_chewing.selectCandidate(index)) {
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
    m_chewing.reset();
    clearComposition();
}

void ZhuyinTextComposer::refreshState()
{
    setPreeditPrefix(m_chewing.preeditPrefix());
    setPreeditText(m_chewing.preeditText());
    setCandidates(m_chewing.candidates());
}

bool ZhuyinTextComposer::commitPendingText()
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    QString text = m_chewing.takeCommitText();
    if (text.isEmpty()) {
        return false;
    }
    engine->commit(text);
    return true;
}

bool ZhuyinTextComposer::commitPreeditBuffer()
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    QString text = m_chewing.takePreeditText();
    if (!text.isEmpty()) {
        engine->commit(text);
    }
    refreshState();
    return !text.isEmpty();
}

QString ZhuyinTextComposer::transformedText(const QString &text) const
{
    if (m_textCase != TextCase::Upper || text.size() != 1) {
        return text;
    }

    return QLocale(inputEngine() ? inputEngine()->locale() : QString()).toUpper(text);
}
