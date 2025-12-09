/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2024 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QKeyEvent>
#include <QObject>
#include <memory>

class InputMethod;
class Keyboard;
class InputMethodContext;

/**
 * High level facade above the underlying classes
 */
class InputPlugin : public QObject
{
    Q_OBJECT
public:
    explicit InputPlugin(InputMethod *inputMethod);
    ~InputPlugin();

    // move to InputPlugin and Camel case
    enum ContentHint {
        content_hint_none = 0x0, // no special behaviour
        content_hint_default = 0x7, // auto completion, correction and capitalization
        content_hint_password = 0xc0, // hidden and sensitive text
        content_hint_auto_completion = 0x1, // suggest word completions
        content_hint_auto_correction = 0x2, // suggest word corrections
        content_hint_auto_capitalization = 0x4, // switch to uppercase letters at the start of a sentence
        content_hint_lowercase = 0x8, // prefer lowercase letters
        content_hint_uppercase = 0x10, // prefer uppercase letters
        content_hint_titlecase = 0x20, // prefer casing for titles and headings (can be language dependent)
        content_hint_hidden_text = 0x40, // characters should be hidden
        content_hint_sensitive_data = 0x80, // typed text should not be stored
        content_hint_latin = 0x100, // just latin characters should be entered
        content_hint_multiline = 0x200, // the text input is multiline
    };
    Q_ENUM(ContentHint)

    enum ContentPurpose {
        content_purpose_normal = 0, // default input, allowing all characters
        content_purpose_alpha = 1, // allow only alphabetic characters
        content_purpose_digits = 2, // allow only digits
        content_purpose_number = 3, // input a number (including decimal separator and sign)
        content_purpose_phone = 4, // input a phone number
        content_purpose_url = 5, // input an URL
        content_purpose_email = 6, // input an email address
        content_purpose_name = 7, // input a name of a person
        content_purpose_password = 8, // input a password (combine with password or sensitive_data hint)
        content_purpose_date = 9, // input a date
        content_purpose_time = 10, // input a time
        content_purpose_datetime = 11, // input a date and time
        content_purpose_terminal = 12, // input for a terminal
    };
    Q_ENUM(ContentPurpose)
    /**
     * Intercept key events
     */
    void setGrabbing(bool grabbing);

    void setPreEditString(const QString &text);
    void moveCursor(int cusorPosition, int anchorPosition);
    void setPreEditCursor(int cursorPosition);
    void setPreEditStyle(int startPosition, int length, int style);
    void deleteSurroundingText(int cursorPos, int length);
    void commit(const QString &text);

    enum KeyState {
        Released = 0,
        Pressed = 1
    };
    void keysym(uint timestamp, uint sym, KeyState state, uint modifiers);

    ContentHint contentHint() const;
    ContentPurpose contentPurpose() const;
    uint32_t cursorPos() const;
    uint32_t anchorPos() const;
    QString surroundingText() const;
    bool hasContext() const
    {
        return m_context.get();
    }

Q_SIGNALS:
    void contextChanged();
    void surroundingTextChanged();
    void cursorChanged();
    void contentTypeChanged();
    void receivedCommit();
    void resetRequested();

    /**
     * A key has been pressed
     * Set the event to accepted to block the key being sent to the client
     */
    void keyPressed(QKeyEvent *keyEvent);
    void keyReleased(QKeyEvent *keyEvent);

protected:
    // move to private when this facade wraps everything

private:
    void setGrabbingInternal();

    bool m_grabbing = false;
    std::shared_ptr<Keyboard> m_keyboard;
    std::shared_ptr<InputMethodContext> m_context = nullptr;
};
