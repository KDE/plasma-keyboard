/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "inputlisteneritem.h"
#include "inputmethod_p.h"

#include <QTextFormat>

#include <set>

#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <qpa/qwindowsysteminterface.h>

Q_GLOBAL_STATIC(InputMethod, s_im)

static const std::set<int> IGNORED_KEYS = {
    Qt::Key_Context1 // Triggered by "special keys" button
};

// Keys to always capture for keyboard navigation
static const QList<int> KEYBOARD_NAVIGATION_CAPTURE_KEYS = {
    Qt::Key_Left,
    Qt::Key_Right,
    Qt::Key_Up,
    Qt::Key_Down,
};

// Keys to capture when keyboard navigation is active
static const QList<int> KEYBOARD_NAVIGATION_ACTIVE_CAPTURE_KEYS = {
    Qt::Key_Return
};

InputListenerItem::InputListenerItem()
    : m_input(&(*s_im))
{
    // Grab and listen to physical keyboard input
    m_input.setGrabbing(true);

    connect(&m_input, &InputPlugin::contextChanged, this, [this] {
        if (m_input.hasContext()) {
            QGuiApplication::inputMethod()->update(Qt::ImQueryAll);
            QGuiApplication::inputMethod()->show();
        } else {
            QGuiApplication::inputMethod()->setVisible(false);
        }
    });
    connect(&m_input, &InputPlugin::surroundingTextChanged, this, [this] {
        QGuiApplication::inputMethod()->update(Qt::ImSurroundingText);

        if (m_input.hasContext()) {
            // Re-activate when text input activates, and there is context
            QGuiApplication::inputMethod()->setVisible(true);
            window()->setVisible(true);
        }
    });
    connect(QGuiApplication::inputMethod(), &QInputMethod::visibleChanged, this, [this] {
        window()->setVisible(QGuiApplication::inputMethod()->isVisible());
    });

    connect(&m_input, &InputPlugin::keyPressed, this, [this](QKeyEvent *keyEvent) {
        if (keyEvent->modifiers() != Qt::NoModifier) {
            return;
        }
        if (!window()->isVisible()) {
            return;
        }

        auto keys = KEYBOARD_NAVIGATION_CAPTURE_KEYS;
        if (m_keyboardNavigationActive) {
            keys.append(KEYBOARD_NAVIGATION_ACTIVE_CAPTURE_KEYS);
        }

        // Forward and accept keyboard navigation events
        for (const auto key : keys) {
            if (keyEvent->key() == key) {
                keyEvent->accept();
                Q_EMIT keyNavigationPressed(key);
                break;
            }
        }
    });
    connect(&m_input, &InputPlugin::keyReleased, this, [this](QKeyEvent *keyEvent) {
        if (keyEvent->modifiers() != Qt::NoModifier) {
            return;
        }
        if (!window()->isVisible()) {
            return;
        }

        auto keys = KEYBOARD_NAVIGATION_CAPTURE_KEYS;
        if (m_keyboardNavigationActive) {
            keys.append(KEYBOARD_NAVIGATION_ACTIVE_CAPTURE_KEYS);
        }

        // Forward and accept keyboard navigation events
        for (const auto key : keys) {
            if (keyEvent->key() == key) {
                keyEvent->accept();
                Q_EMIT keyNavigationReleased(key);
                break;
            }
        }
    });

    // Don't hook into the &InputPlugin::receivedCommit signal and call setVisible(true)
    // -> it can cause a race condition as receivedCommit is emitted when the text field loses focus.
    //
    // If the virtual keyboard (vkbd) is manually closed and then the text field loses focus,
    // setVisible(true/false) calls in quick succession may reopen the vkbd in a broken state.
    //
    // Series of events:
    // - vkbd is manually closed (by user), but text field still focused -> setVisible(false)
    // - User unfocuses the text field
    // - receivedCommit() emitted -> setVisible(true)
    // - contextChanged() (m_input.hasContext() = false) -> setVisible(false)
    // - keyboard gets reopened by KWin because the input context gets from setVisible(true) finishes initializing!
    //
    // This happens because of a delay in InputPanelV1Window initialization, which confuses KWin
    // into creating a new input context even when no text field is focused.
    //
    // TODO: Investigate other places this might happen and how to fix it.

    QGuiApplication::inputMethod()->update(Qt::ImQueryAll);
}

void InputListenerItem::setEngine(QVirtualKeyboardInputEngine *engine) {
    // TODO: hook into engine events if necessary?
}

void InputListenerItem::setKeyboardNavigationActive(bool keyboardNavigationActive)
{
    m_keyboardNavigationActive = keyboardNavigationActive;
}

QVariant InputListenerItem::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (!m_input.hasContext()) {
        return {};
    }

    switch (query) {
    case Qt::ImEnabled:
        return true;
    case Qt::ImSurroundingText:
        return m_input.surroundingText();
    case Qt::ImHints: {
        const auto imHints = m_input.contentHint();
        Qt::InputMethodHints qtHints;

        // if (imHints & InputPlugin::content_hint_default) { }
        if (imHints & InputPlugin::content_hint_password) {
            qtHints |= Qt::ImhSensitiveData;
        }
        if ((imHints & InputPlugin::content_hint_auto_completion) == 0) {
            qtHints |= Qt::ImhNoPredictiveText;
        }
        if ((imHints & InputPlugin::content_hint_auto_correction) == 0 || (imHints & InputPlugin::content_hint_auto_capitalization) == 0) {
            qtHints |= Qt::ImhNoAutoUppercase;

        }
        // if (imHints & InputPlugin::content_hint_titlecase) { }
        if (imHints & InputPlugin::content_hint_lowercase) {
            qtHints |= Qt::ImhPreferLowercase;
        }
        if (imHints & InputPlugin::content_hint_uppercase) {
            qtHints |= Qt::ImhPreferUppercase;
        }
        if (imHints & InputPlugin::content_hint_hidden_text) {
            qtHints |= Qt::ImhSensitiveData;
        }
        if (imHints & InputPlugin::content_hint_sensitive_data) {
            qtHints |= Qt::ImhSensitiveData;
        }
        if (imHints & InputPlugin::content_hint_latin) {
            qtHints |= Qt::ImhPreferLatin;
        }
        if (imHints & InputPlugin::content_hint_multiline) {
            qtHints |= Qt::ImhMultiLine;
        }
        const auto imPurpose = m_input.contentPurpose();
        switch (imPurpose) {
            case InputPlugin::content_purpose_normal:
            case InputPlugin::content_purpose_alpha:
            case InputPlugin::content_purpose_name:
                break;
            case InputPlugin::content_purpose_digits:
                qtHints |= Qt::ImhDigitsOnly;
                break;
            case InputPlugin::content_purpose_number:
                qtHints |= Qt::ImhPreferNumbers;
                break;
            case InputPlugin::content_purpose_phone:
                qtHints |= Qt::ImhDialableCharactersOnly;
                break;
            case InputPlugin::content_purpose_url:
                qtHints |= Qt::ImhUrlCharactersOnly;
                break;
            case InputPlugin::content_purpose_email:
                qtHints |= Qt::ImhEmailCharactersOnly;
                break;
            case InputPlugin::content_purpose_password:
                qtHints |= Qt::ImhSensitiveData;
                break;
            case InputPlugin::content_purpose_date:
                qtHints |= Qt::ImhDate;
                break;
            case InputPlugin::content_purpose_time:
                qtHints |= Qt::ImhTime;
                break;
            case InputPlugin::content_purpose_datetime:
                qtHints |= Qt::ImhDate;
                qtHints |= Qt::ImhTime;
                break;
            case InputPlugin::content_purpose_terminal:
                qtHints |= Qt::ImhPreferLatin;
                break;
        }
        return QVariant::fromValue<int>(qtHints);
    } break;
    case Qt::ImCurrentSelection:
        return m_input.surroundingText().mid(m_input.cursorPos(), m_input.anchorPos());
    case Qt::ImAnchorPosition:
    case Qt::ImAnchorRectangle:
    case Qt::ImCursorPosition:
    case Qt::ImCursorRectangle:
    case Qt::ImInputItemClipRectangle:
        // We don't do that
        break;
    default:
        qWarning() << "Unhandled query" << query;
        break;
    }
    return {};
}

void InputListenerItem::keyPressEvent(QKeyEvent *event)
{
    if (IGNORED_KEYS.find(event->key()) != IGNORED_KEYS.end()) {
        return;
    }

    QList<xkb_keysym_t> keys = QXkbCommon::toKeysym(event);
    for (auto key : keys) {
        // Simulate key press only if it's not textual
        if (event->text().isEmpty() || key == XKB_KEY_Return) { // (return is technically "\n")
            m_input.keysym(QDateTime::currentMSecsSinceEpoch(), key, InputPlugin::Pressed, 0);
        }
    }
}

void InputListenerItem::keyReleaseEvent(QKeyEvent *event)
{
    if (IGNORED_KEYS.find(event->key()) != IGNORED_KEYS.end()) {
        return;
    }

    QList<xkb_keysym_t> keys = QXkbCommon::toKeysym(event);
    for (auto key : keys) {
        if (event->text().isEmpty() || key == XKB_KEY_Return) { // (return is technically "\n")
            // Simulate the keyboard press for non textual keys
            m_input.keysym(QDateTime::currentMSecsSinceEpoch(), key, InputPlugin::Released, 0);
        } else {
            // If we have text coming as a key event, use it to commit the string
            m_input.commit(event->text());
        }
    }
}

void InputListenerItem::inputMethodEvent(QInputMethodEvent *event)
{
    // Delete characters that are supposed to be replaced
    if (event->replacementLength() > 0) {
        m_input.deleteSurroundingText(event->replacementStart(), event->replacementLength());
    }

    for (auto x : event->attributes()) {
        if (x.type == QInputMethodEvent::TextFormat) {
            m_input.setPreEditStyle(x.start, x.length, x.value.value<QTextFormat>().type());
        }
    }

    // Send cursor position (must be before predit string)
    m_input.setPreEditCursor(event->preeditString().size());

    // Send currently being edited string
    m_input.setPreEditString(event->preeditString());

    // Commit string if we have a finished string
    if (const auto commit = event->commitString(); !commit.isEmpty()) {
        m_input.commit(commit);
    }
}
