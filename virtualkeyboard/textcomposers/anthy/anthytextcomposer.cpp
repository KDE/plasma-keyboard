/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "anthytextcomposer.h"

#include "inputengine.h"

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

    commitComposition();
    m_anthy.setKatakana(inputMode == u"katakana"_s);
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

    QString typedText = applyCapitalization(text);
    if (isLatinMode() || !m_anthy.initialize()) {
        return engine->handleKeyCommit(key, typedText);
    }

    if (m_anthy.hasComposition()) {
        switch (key) {
        case Qt::Key_Backspace:
            // Backspace goes back to the reading before it starts deleting it
            if (m_anthy.isConverting()) {
                m_anthy.cancelConversion();
            } else {
                m_anthy.removeLastInput();
            }
            refreshState();
            return true;
        case Qt::Key_Escape:
            if (m_anthy.isConverting()) {
                m_anthy.cancelConversion();
                refreshState();
            } else {
                reset();
            }
            return true;
        case Qt::Key_Space:
            if (m_anthy.selectNextCandidate()) {
                refreshState();
                return true;
            }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            return commitComposition();
        default:
            break;
        }
    }

    // Typing after choosing a candidate keeps it, and starts a new reading
    if (m_anthy.isConverting() && AnthyAdapter::acceptsText(typedText)) {
        commitComposition();
    }
    if (m_anthy.appendText(typedText)) {
        refreshState();
        return true;
    }

    const bool committed = commitComposition();
    return engine->handleKeyCommit(key, typedText) || committed;
}

bool AnthyTextComposer::selectCandidate(int index)
{
    if (!m_anthy.selectCandidate(index)) {
        return false;
    }
    return commitComposition();
}

bool AnthyTextComposer::replaceLastInput(const QString &text)
{
    if (isLatinMode()) {
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

bool AnthyTextComposer::showsPreeditBubble() const
{
    return true;
}

void AnthyTextComposer::reset()
{
    m_anthy.reset();
    clearComposition();
}

bool AnthyTextComposer::isLatinMode() const
{
    return m_inputMode == u"latin"_s;
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
    auto *engine = inputEngine();
    if (text.isEmpty() || !engine) {
        clearComposition();
        return false;
    }
    engine->commit(text);
    return true;
}

QString AnthyTextComposer::applyCapitalization(const QString &text) const
{
    if (m_textCase != TextCase::Upper || text.size() != 1) {
        return text;
    }

    return QLocale(inputEngine() ? inputEngine()->locale() : QString()).toUpper(text);
}
