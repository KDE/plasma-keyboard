/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "vkbdwaylandinputbackend.h"

VkbdWaylandInputBackend::VkbdWaylandInputBackend(QObject *parent)
    : InputBackend(parent)
{
}

bool VkbdWaylandInputBackend::isActive() const
{
    return m_active;
}

Qt::InputMethodHints VkbdWaylandInputBackend::inputMethodHints() const
{
    return m_inputMethodHints;
}

QString VkbdWaylandInputBackend::surroundingText() const
{
    return m_surroundingText;
}

uint32_t VkbdWaylandInputBackend::cursorPositionUtf8() const
{
    return m_cursorPositionUtf8;
}

uint32_t VkbdWaylandInputBackend::anchorPositionUtf8() const
{
    return m_anchorPositionUtf8;
}

void VkbdWaylandInputBackend::setPreeditText(const QString &text)
{
    Q_EMIT preeditTextRequested(text);
}

void VkbdWaylandInputBackend::commitText(const QString &text)
{
    Q_EMIT commitTextRequested(text);
}

void VkbdWaylandInputBackend::deleteSurroundingText(int index, int length)
{
    Q_EMIT deleteSurroundingTextRequested(index, length);
}

bool VkbdWaylandInputBackend::sendKeyClick(int key)
{
    bool handled = false;
    Q_EMIT keyClickRequested(key, &handled);
    return handled;
}

bool VkbdWaylandInputBackend::sendKeyPressed(int key, bool pressed)
{
    bool handled = false;
    Q_EMIT keyPressedRequested(key, pressed, &handled);
    if (handled) {
        setKeyPressed(key, pressed);
    }
    return handled;
}

void VkbdWaylandInputBackend::setActive(bool active)
{
    if (m_active == active) {
        return;
    }

    m_active = active;
    Q_EMIT activeChanged();
}

void VkbdWaylandInputBackend::setInputMethodHints(Qt::InputMethodHints hints)
{
    if (m_inputMethodHints == hints) {
        return;
    }

    m_inputMethodHints = hints;
    Q_EMIT inputMethodHintsChanged();
}

void VkbdWaylandInputBackend::setSurroundingState(const QString &text, uint32_t cursorPositionUtf8, uint32_t anchorPositionUtf8)
{
    if (m_surroundingText == text && m_cursorPositionUtf8 == cursorPositionUtf8 && m_anchorPositionUtf8 == anchorPositionUtf8) {
        return;
    }

    m_surroundingText = text;
    m_cursorPositionUtf8 = cursorPositionUtf8;
    m_anchorPositionUtf8 = anchorPositionUtf8;
    Q_EMIT surroundingTextChanged();
}
