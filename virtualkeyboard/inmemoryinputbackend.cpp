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
    return m_text.first(m_cursor) + m_preeditText + m_text.sliced(m_cursor);
}

int InMemoryInputBackend::displayCursorPosition() const
{
    return m_cursor + m_preeditText.size();
}

void InMemoryInputBackend::clear()
{
    const QString oldDisplayText = displayText();
    const int oldDisplayCursorPosition = displayCursorPosition();
    m_preeditText.clear();
    m_text.clear();
    m_cursor = 0;
    if (oldDisplayText != displayText()) {
        Q_EMIT surroundingTextChanged();
        Q_EMIT displayTextChanged();
    }
    if (oldDisplayCursorPosition != displayCursorPosition()) {
        Q_EMIT displayCursorPositionChanged();
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
    return QStringView(m_text).first(m_cursor).toUtf8().size();
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
    Q_EMIT displayCursorPositionChanged();
}

void InMemoryInputBackend::commitText(const QString &text)
{
    if (text.isEmpty()) {
        return;
    }

    QString newText = m_text;
    newText.insert(m_cursor, text);
    setState(newText, m_cursor + text.size());
}

void InMemoryInputBackend::deleteSurroundingText(int index, int length)
{
    // index is relative to the cursor
    const qsizetype start = qBound<qsizetype>(0, m_cursor + qsizetype(index), m_text.size());
    const qsizetype count = qBound<qsizetype>(0, qsizetype(length), m_text.size() - start);
    if (count == 0) {
        return;
    }

    QString text = m_text;
    text.remove(start, count);

    // Keep the cursor in place, unless text before (or around) it was removed
    qsizetype cursor = m_cursor;
    if (start < cursor) {
        cursor -= qMin(count, cursor - start);
    }
    setState(text, cursor);
}

bool InMemoryInputBackend::sendKeyClick(int key)
{
    if (key == Qt::Key_Space) {
        commitText(QStringLiteral(" "));
        return true;
    }
    return handleEditingKey(key);
}

bool InMemoryInputBackend::sendKeyPressed(int key, bool pressed)
{
    switch (key) {
    case Qt::Key_Backspace:
    case Qt::Key_Delete:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Home:
    case Qt::Key_End:
        return !pressed || handleEditingKey(key);
    default:
        return false;
    }
}

bool InMemoryInputBackend::handleEditingKey(int key)
{
    switch (key) {
    case Qt::Key_Backspace: {
        const qsizetype boundary = previousGraphemeBoundary();
        deleteSurroundingText(boundary - m_cursor, m_cursor - boundary);
        return true;
    }
    case Qt::Key_Delete:
        deleteSurroundingText(0, nextGraphemeBoundary() - m_cursor);
        return true;
    case Qt::Key_Left:
        moveCursor(previousGraphemeBoundary());
        return true;
    case Qt::Key_Right:
        moveCursor(nextGraphemeBoundary());
        return true;
    case Qt::Key_Home:
        moveCursor(0);
        return true;
    case Qt::Key_End:
        moveCursor(m_text.size());
        return true;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Tab:
    case Qt::Key_Up:
    case Qt::Key_Down:
        return true;
    default:
        return false;
    }
}

void InMemoryInputBackend::setState(const QString &text, qsizetype cursor)
{
    const bool textChanged = m_text != text;
    const bool cursorChanged = m_cursor != cursor;
    if (!textChanged && !cursorChanged) {
        return;
    }

    m_text = text;
    m_cursor = cursor;

    Q_EMIT surroundingTextChanged();
    if (textChanged) {
        Q_EMIT displayTextChanged();
    }
    if (cursorChanged) {
        Q_EMIT displayCursorPositionChanged();
    }
}

void InMemoryInputBackend::moveCursor(qsizetype cursor)
{
    commitPreedit();
    setState(m_text, qBound<qsizetype>(0, cursor, m_text.size()));
}

void InMemoryInputBackend::commitPreedit()
{
    if (m_preeditText.isEmpty()) {
        return;
    }

    m_text.insert(m_cursor, m_preeditText);
    m_cursor += m_preeditText.size();
    m_preeditText.clear();
    Q_EMIT surroundingTextChanged();

    Q_EMIT resetRequested();
}

qsizetype InMemoryInputBackend::previousGraphemeBoundary() const
{
    QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, m_text);
    finder.setPosition(m_cursor);
    return qMax<qsizetype>(0, finder.toPreviousBoundary());
}

qsizetype InMemoryInputBackend::nextGraphemeBoundary() const
{
    QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, m_text);
    finder.setPosition(m_cursor);
    const qsizetype boundary = finder.toNextBoundary();
    return boundary < 0 ? m_text.size() : boundary;
}
