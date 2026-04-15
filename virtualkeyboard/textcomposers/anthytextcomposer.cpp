/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "anthytextcomposer.h"

#include "../inputengine.h"

#include <QLocale>

using namespace Qt::StringLiterals;

AnthyTextComposer::AnthyTextComposer(QObject *parent)
    : AbstractTextComposer(parent)
{
}

AnthyTextComposer::~AnthyTextComposer() = default;

QString AnthyTextComposer::inputMode() const
{
    return m_inputMode;
}

void AnthyTextComposer::setInputMode(const QString &inputMode)
{
    if (m_inputMode == inputMode || inputMode.isEmpty()) {
        return;
    }

    m_anthy.setInputMode(inputMode);
    clearComposition();
    m_inputMode = inputMode;
    Q_EMIT inputModeChanged();
}

bool AnthyTextComposer::cycleKanaModifier()
{
    if (!m_anthy.cycleKanaModifier()) {
        return false;
    }
    refreshState();
    return true;
}

bool AnthyTextComposer::autoCapitalizationSupported() const
{
    return false;
}

bool AnthyTextComposer::setTextCase(TextCase textCase)
{
    m_textCase = textCase;
    return true;
}

bool AnthyTextComposer::keyEvent(Qt::Key key, const QString &text)
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    QString textToCommit = applyCapitalization(text);
    if (inputMode() == u"latin"_s || !m_anthy.initialize()) {
        return engine->handleKeyCommit(key, textToCommit);
    }

    switch (key) {
    case Qt::Key_Backspace:
        if (!m_anthy.hasComposition()) {
            return engine->handleKeyCommit(key, textToCommit);
        }
        m_anthy.removeLastInput();
        refreshState();
        return true;
    case Qt::Key_Space:
        if (!m_anthy.hasComposition()) {
            return engine->handleKeyCommit(key, textToCommit);
        }
        m_anthy.cycleCandidate();
        refreshState();
        return true;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (!m_anthy.hasComposition()) {
            return engine->handleKeyCommit(key, textToCommit);
        }
        return commitComposition();
    case Qt::Key_Escape:
        if (!m_anthy.hasComposition()) {
            return false;
        }
        reset();
        return true;
    default:
        break;
    }

    if (m_anthy.appendText(textToCommit)) {
        refreshState();
        return true;
    }
    bool committed = m_anthy.hasComposition() && commitComposition();
    return engine->handleKeyCommit(key, textToCommit) || committed;
}

bool AnthyTextComposer::selectCandidate(int index)
{
    if (!m_anthy.selectCandidate(index)) {
        return false;
    }
    return commitComposition();
}

bool AnthyTextComposer::showsPreeditBubble() const
{
    return true;
}

void AnthyTextComposer::reset()
{
    m_anthy.reset();
    clearComposition();
}

void AnthyTextComposer::refreshState()
{
    setPreeditPrefix(QString());
    setPreeditText(m_anthy.preeditText());
    setCandidates(m_anthy.candidates());
}

bool AnthyTextComposer::commitComposition()
{
    QString text = m_anthy.takeCommitText();
    if (text.isEmpty()) {
        clearComposition();
        return false;
    }
    if (auto *engine = inputEngine()) {
        engine->commit(text);
    }
    return true;
}

QString AnthyTextComposer::applyCapitalization(const QString &text) const
{
    if (m_textCase != TextCase::Upper || text.size() != 1) {
        return text;
    }

    return QLocale(inputEngine() ? inputEngine()->locale() : QString()).toUpper(text);
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

    if (!m_anthy.replaceLastInput(text)) {
        return false;
    }
    refreshState();
    return true;
}
