/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "inmemoryinputbackend.h"

#include <QTextBoundaryFinder>

InMemoryInputBackend::InMemoryInputBackend(QObject *parent)
    : InputBackend(parent)
{
}

QString InMemoryInputBackend::displayText() const
{
    return m_text + m_preeditText;
}

void InMemoryInputBackend::clear()
{
    const QString oldDisplayText = displayText();
    m_preeditText.clear();
    m_text.clear();
    if (oldDisplayText != displayText()) {
        Q_EMIT surroundingTextChanged();
        Q_EMIT displayTextChanged();
    }
}

bool InMemoryInputBackend::isActive() const
{
    return true;
}

Qt::InputMethodHints InMemoryInputBackend::inputMethodHints() const
{
    return Qt::ImhNoAutoUppercase | Qt::ImhPreferLowercase | Qt::ImhNoPredictiveText;
}

QString InMemoryInputBackend::surroundingText() const
{
    return m_text;
}

uint32_t InMemoryInputBackend::cursorPositionUtf8() const
{
    // The cursor is always at the end of the text
    return m_text.toUtf8().size();
}

uint32_t InMemoryInputBackend::anchorPositionUtf8() const
{
    return cursorPositionUtf8();
}

void InMemoryInputBackend::setPreeditText(const QString &text)
{
    if (m_preeditText == text) {
        return;
    }

    m_preeditText = text;
    Q_EMIT displayTextChanged();
}

void InMemoryInputBackend::commitText(const QString &text)
{
    if (text.isEmpty()) {
        return;
    }

    setText(m_text + text);
}

void InMemoryInputBackend::deleteSurroundingText(int index, int length)
{
    // index is relative to the cursor, which is at the end of the text
    const qsizetype start = qBound<qsizetype>(0, m_text.size() + qsizetype(index), m_text.size());
    const qsizetype count = qBound<qsizetype>(0, qsizetype(length), m_text.size() - start);
    if (count == 0) {
        return;
    }

    QString text = m_text;
    text.remove(start, count);
    setText(text);
}

bool InMemoryInputBackend::sendKeyClick(int key)
{
    switch (key) {
    case Qt::Key_Backspace:
        removePreviousGrapheme();
        return true;
    case Qt::Key_Space:
        commitText(QStringLiteral(" "));
        return true;
    case Qt::Key_Delete:
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Tab:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Home:
    case Qt::Key_End:
        // The cursor is fixed at the end of a single-line field
        return true;
    default:
        return false;
    }
}

bool InMemoryInputBackend::sendKeyPressed(int key, bool pressed)
{
    Q_UNUSED(key)
    Q_UNUSED(pressed)
    return false;
}

void InMemoryInputBackend::setText(const QString &text)
{
    if (m_text == text) {
        return;
    }

    m_text = text;
    Q_EMIT surroundingTextChanged();
    Q_EMIT displayTextChanged();
}

void InMemoryInputBackend::removePreviousGrapheme()
{
    if (m_text.isEmpty()) {
        return;
    }

    QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, m_text);
    finder.toEnd();
    const qsizetype previousBoundary = finder.toPreviousBoundary();
    setText(previousBoundary < 0 ? QString() : m_text.first(previousBoundary));
}
