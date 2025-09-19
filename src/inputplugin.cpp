/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2024 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "inputplugin.h"
#include "inputmethod_p.h"

InputPlugin::InputPlugin(InputMethod *inputMethod)
{
    connect(inputMethod, &InputMethod::activityChanged, this, [this, inputMethod]() {
        if (m_context) {
            disconnect(m_context.get(), nullptr, this, nullptr);
        }
        m_keyboard.reset();
        m_context = inputMethod->current();
        Q_EMIT contextChanged();
        Q_EMIT surroundingTextChanged();
        if (!m_context) {
            return;
        }

        connect(m_context.get(), &InputMethodContext::receivedCommit, this, &InputPlugin::receivedCommit);
        connect(m_context.get(), &InputMethodContext::surroundingTextChanged, this, &InputPlugin::surroundingTextChanged);
        connect(m_context.get(), &InputMethodContext::contentTypeChanged, this, &InputPlugin::contentTypeChanged);
        connect(m_context.get(), &InputMethodContext::reset, this, &InputPlugin::resetRequested);
        if (m_grabbing) {
            setGrabbingInternal();
        }
    });
}

InputPlugin::~InputPlugin()
{
}

void InputPlugin::setGrabbing(bool grabbing)
{
    m_grabbing = grabbing;
    if (m_context && grabbing) {
        setGrabbingInternal();
    }
    if (m_keyboard && !grabbing) {
        disconnect(m_keyboard.get(), nullptr, this, nullptr);
        m_keyboard.reset();
    }
}

void InputPlugin::setPreEditString(const QString &text)
{
    if (!m_context) {
        return;
    }
    m_context->preedit_string(m_context->m_latestSerial, text, text);
}

void InputPlugin::moveCursor(int cusorPosition, int anchorPosition)
{
    if (!m_context) {
        return;
    }
    m_context->cursor_position(cusorPosition, anchorPosition);
}

void InputPlugin::setPreEditCursor(int cursorPosition)
{
    if (!m_context) {
        return;
    }
    m_context->preedit_cursor(cursorPosition);
}

void InputPlugin::setPreEditStyle(int startPosition, int length, int style)
{
    if (!m_context) {
        return;
    }
    m_context->preedit_styling(startPosition, length, style);
}

void InputPlugin::deleteSurroundingText(int index, int length)
{
    if (!m_context) {
        return;
    }
    m_context->delete_surrounding_text(index, length);
}

void InputPlugin::commit(const QString &text)
{
    if (!m_context) {
        return;
    }
    m_context->commit_string(m_context->m_latestSerial, text);
}

void InputPlugin::keysym(uint timestamp, uint sym, KeyState state, uint modifiers)
{
    if (!m_context) {
        return;
    }
    m_context->keysym(m_context->m_latestSerial, timestamp, sym, state, modifiers);
}

InputPlugin::ContentHint InputPlugin::contentHint() const
{
    if (!m_context) {
        return InputPlugin::content_hint_none;
    }
    return m_context->m_contentHint;
}

InputPlugin::ContentPurpose InputPlugin::contentPurpose() const
{
    if (!m_context) {
        return InputPlugin::content_purpose_normal;
    }
    return m_context->m_contentPurpose;
}

QString InputPlugin::surroundingText() const
{
    if (!m_context) {
        return QString();
    }
    return m_context->m_text;
}

uint32_t InputPlugin::cursorPos() const
{
    if (!m_context) {
        return 0;
    }
    return m_context->m_cursor;
}

uint32_t InputPlugin::anchorPos() const
{
    if (!m_context) {
        return 0;
    }
    return m_context->m_anchor;
}

void InputPlugin::setGrabbingInternal()
{
    Q_ASSERT(m_context);
    m_keyboard = m_context->keyboard();
    connect(m_keyboard.get(), &Keyboard::keyPressed, this, &InputPlugin::keyPressed);
    connect(m_keyboard.get(), &Keyboard::keyReleased, this, &InputPlugin::keyReleased);
}

#include "moc_inputplugin.cpp"
