/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "inputlisteneritem.h"
#include "inputmethod_p.h"
#include "plasmakeyboardsettings.h"

#include <QTextFormat>

#include <set>

#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <qpa/qwindowsysteminterface.h>

Q_GLOBAL_STATIC(InputMethod, s_im)

Q_GLOBAL_STATIC_WITH_ARGS(const std::set<int>,
                          IGNORED_KEYS,
                          {
                              Qt::Key_Context1 // Triggered by "special keys" button
                          });
// Keys to always capture for keyboard navigation
QList<Qt::Key> initCapture()
{
    return {
        Qt::Key_Left,
        Qt::Key_Right,
        Qt::Key_Up,
        Qt::Key_Down,
    };
}
Q_GLOBAL_STATIC_WITH_ARGS(const QList<Qt::Key>, KEYBOARD_NAVIGATION_CAPTURE_KEYS, (initCapture()));

// Keys to capture when keyboard navigation is active
Q_GLOBAL_STATIC_WITH_ARGS(const QList<Qt::Key>, KEYBOARD_NAVIGATION_ACTIVE_CAPTURE_KEYS, (initCapture() + QList<Qt::Key>{Qt::Key_Return}));

InputListenerItem::InputListenerItem()
    : m_input(&(*s_im))
{
    // Grab and listen to physical keyboard input
    m_input.setGrabbing(true);

    m_holdTimer.setSingleShot(true);
    connect(&m_holdTimer, &QTimer::timeout, this, &InputListenerItem::handleHoldTimeout);

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
    connect(&m_input, &InputPlugin::resetRequested, this, [] {
        QGuiApplication::inputMethod()->reset();
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

        if (PlasmaKeyboardSettings::self()->keyboardNavigationEnabled()) {
            // Keys to capture for keyboard navigation
            const auto keys = m_keyboardNavigationActive ? *KEYBOARD_NAVIGATION_ACTIVE_CAPTURE_KEYS : *KEYBOARD_NAVIGATION_CAPTURE_KEYS;

            // Forward and accept keyboard navigation events
            for (const auto key : keys) {
                if (keyEvent->key() == key) {
                    keyEvent->accept();
                    Q_EMIT keyNavigationPressed(key);
                    break;
                }
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

        if (PlasmaKeyboardSettings::self()->keyboardNavigationEnabled()) {
            // Keys to capture for keyboard navigation
            const auto keys = m_keyboardNavigationActive ? *KEYBOARD_NAVIGATION_ACTIVE_CAPTURE_KEYS : *KEYBOARD_NAVIGATION_CAPTURE_KEYS;

            // Forward and accept keyboard navigation events
            for (const auto key : keys) {
                if (keyEvent->key() == key) {
                    keyEvent->accept();
                    Q_EMIT keyNavigationReleased(key);
                    break;
                }
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

void InputListenerItem::setEngine(QVirtualKeyboardInputEngine * /*engine*/)
{
    // TODO: hook into engine events if necessary?
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
    }
    case Qt::ImCurrentSelection: {
        // cursorPos and anchorPos are in bytes, we need to convert QString to QByteArray for index operations
        QByteArray surroundingText = m_input.surroundingText().toUtf8();
        return QString::fromUtf8(surroundingText.mid(m_input.cursorPos(), m_input.anchorPos()));
    }
    case Qt::ImAnchorPosition:
        return m_input.anchorPos();
    case Qt::ImCursorPosition:
        return m_input.cursorPos();
    case Qt::ImTextBeforeCursor: {
        // cursorPos is in bytes, we need to convert QString to QByteArray for index operations
        QByteArray surroundingText = m_input.surroundingText().toUtf8();
        return QString::fromUtf8(surroundingText.first(m_input.cursorPos()));
    }
    case Qt::ImTextAfterCursor: {
        // cursorPos is in bytes, we need to convert QString to QByteArray for index operations
        QByteArray surroundingText = m_input.surroundingText().toUtf8();
        return QString::fromUtf8(surroundingText.mid(m_input.cursorPos() + 1));
    }
    case Qt::ImCursorRectangle:
    case Qt::ImFont:
    case Qt::ImMaximumTextLength:
    case Qt::ImPreferredLanguage:
    case Qt::ImPlatformData:
    case Qt::ImAbsolutePosition:
    case Qt::ImEnterKeyType:
    case Qt::ImAnchorRectangle:
    case Qt::ImInputItemClipRectangle:
    case Qt::ImReadOnly:
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
    if (IGNORED_KEYS->find(event->key()) != IGNORED_KEYS->end()) {
        return;
    }

    // Handle diacritics detection for textual keys
    if (shouldHandleDiacritics(event)) {
        resetPendingDiacriticsState();
        m_pendingText = event->text();
        m_pendingKey = event->key();
        m_selectionMade = false;
        m_popupShown = false;
        m_popupDismissed = false;

        const int holdThreshold = PlasmaKeyboardSettings::self()->diacriticsHoldThresholdMs();
        m_holdTimer.start(qMax(holdThreshold, 0));
        event->accept();
        return;
    }

    const QList<xkb_keysym_t> keys = QXkbCommon::toKeysym(event);
    for (auto key : keys) {
        // Simulate key press only if it's not textual
        if (event->text().isEmpty() || key == XKB_KEY_Return) { // (return is technically "\n")
            m_input.keysym(QDateTime::currentMSecsSinceEpoch(), key, InputPlugin::Pressed, 0);
        }
    }
}

void InputListenerItem::keyReleaseEvent(QKeyEvent *event)
{
    if (IGNORED_KEYS->find(event->key()) != IGNORED_KEYS->end()) {
        return;
    }

    if (!m_pendingText.isEmpty() && event->key() == m_pendingKey && !event->isAutoRepeat()) {
        // Stop long-press timer if still counting
        if (m_holdTimer.isActive()) {
            m_holdTimer.stop();
        }

        if (m_popupShown) {
            // Popup was shown; if user did not pick an alternative, commit the base character
            if (!m_selectionMade && !m_pendingText.isEmpty()) {
                m_input.commit(m_pendingText);
            }
            if (!m_popupDismissed) {
                Q_EMIT diacriticsPopupCancelled();
            }
            resetPendingDiacriticsState();
            event->accept();
            return;
        }

        // No popup, commit the base character now
        m_input.commit(m_pendingText);
        resetPendingDiacriticsState();
        event->accept();
        return;
    }

    const QList<xkb_keysym_t> keys = QXkbCommon::toKeysym(event);
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

    const auto attributes = event->attributes();
    for (const auto &x : attributes) {
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

void InputListenerItem::handleHoldTimeout()
{
    if (m_pendingText.isEmpty()) {
        return;
    }

    m_popupShown = true;
    m_popupDismissed = false;
    Q_EMIT diacriticsPopupRequested(m_pendingText);
}

void InputListenerItem::commitDiacritic(const QString &text)
{
    if (text.isEmpty()) {
        return;
    }

    m_selectionMade = true;
    m_input.commit(text);
    if (m_popupShown && !m_popupDismissed) {
        Q_EMIT diacriticsPopupCancelled();
        m_popupDismissed = true;
    }
}

bool InputListenerItem::shouldHandleDiacritics(const QKeyEvent *event) const
{
    if (!PlasmaKeyboardSettings::self()->diacriticsPopupEnabled()) {
        return false;
    }

    if (event->isAutoRepeat()) {
        return false;
    }

    // Only handle simple textual keys without control/meta modifiers
    const Qt::KeyboardModifiers mods = event->modifiers();
    const bool modifierAllowed = mods == Qt::NoModifier || mods == Qt::ShiftModifier;
    if (!modifierAllowed) {
        return false;
    }

    return !event->text().isEmpty() && event->text().size() == 1;
}

void InputListenerItem::resetPendingDiacriticsState()
{
    if (m_holdTimer.isActive()) {
        m_holdTimer.stop();
    }
    m_pendingText.clear();
    m_pendingKey = 0;
    m_popupShown = false;
    m_popupDismissed = false;
    m_selectionMade = false;
}

#include "moc_inputlisteneritem.cpp"
