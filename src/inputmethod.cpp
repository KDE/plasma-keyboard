/*
    SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "inputmethod_p.h"
#include <QDateTime>
#include <QDebug>
#include <QGuiApplication>
#include <QInputMethod>
#include <QKeyEvent>
#include <QStandardPaths>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

InputMethod::InputMethod()
    : QWaylandClientExtensionTemplate<InputMethod>(1)
{
}

InputMethod::~InputMethod() = default;

void InputMethod::zwp_input_method_v1_activate(struct ::zwp_input_method_context_v1 *id)
{
    setCurrentContext(new InputMethodContext(id));
}

void InputMethod::zwp_input_method_v1_deactivate(struct ::zwp_input_method_context_v1 *context)
{
    Q_ASSERT(m_current->object() == context);
    setCurrentContext(nullptr);
}

void InputMethod::setCurrentContext(InputMethodContext *context)
{
    if (m_current.get() == context) {
        return;
    }
    m_current.reset(context);
    Q_EMIT activityChanged(m_current.use_count());

    if (m_current)
        Q_EMIT activate();
    else
        Q_EMIT deactivate();
}

InputMethodContext::InputMethodContext(struct ::zwp_input_method_context_v1 *id)
    : QtWayland::zwp_input_method_context_v1(id)
{
}

InputMethodContext::~InputMethodContext() = default;

void InputMethodContext::zwp_input_method_context_v1_reset()
{
    Q_EMIT reset();
}

void InputMethodContext::zwp_input_method_context_v1_commit_state(uint32_t serial)
{
    m_latestSerial = serial;
    Q_EMIT receivedCommit();
}

void InputMethodContext::zwp_input_method_context_v1_content_type(uint32_t hint, uint32_t purpose)
{
    m_contentHint = InputPlugin::ContentHint(hint);
    m_contentPurpose = InputPlugin::ContentPurpose(purpose);
    Q_EMIT contentTypeChanged();
}

void InputMethodContext::zwp_input_method_context_v1_preferred_language(const QString &language)
{
    Q_EMIT preferredLanguageChanged(language);
}

void InputMethodContext::zwp_input_method_context_v1_surrounding_text(const QString &text, uint32_t cursor, uint32_t anchor)
{
    m_text = text;
    m_cursor = cursor;
    m_anchor = anchor;
    Q_EMIT surroundingTextChanged(text, cursor, anchor);
}

void InputMethodContext::zwp_input_method_context_v1_invoke_action(uint32_t button, uint32_t index)
{
}

std::shared_ptr<Keyboard> InputMethodContext::keyboard()
{
    if (auto existing = m_keyboard.lock()) {
        return existing;
    }

    auto wlKeyboard = grab_keyboard();
    auto keyboard = std::make_shared<Keyboard>(wlKeyboard, this);
    m_keyboard = keyboard;
    return keyboard;
}

Keyboard::Keyboard(::wl_keyboard *keyboard, InputMethodContext *parent)
    : wl_keyboard(keyboard)
    , m_parent(parent)
{
    mXkbContext.reset(xkb_context_new(XKB_CONTEXT_NO_FLAGS));
}

Keyboard::~Keyboard()
{
    // needs version guarding, and the versioning here is messed up here, unless we're in sync with seat :/
    //        release();
}

void Keyboard::keyboard_keymap(uint32_t format, int32_t fd, uint32_t size)
{
    mKeymapFormat = format;
    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        return;
    }

    char *map_str = static_cast<char *>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (map_str == MAP_FAILED) {
        close(fd);
        return;
    }

    mXkbKeymap.reset(xkb_keymap_new_from_string(mXkbContext.get(), map_str, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS));
    QXkbCommon::verifyHasLatinLayout(mXkbKeymap.get());

    munmap(map_str, size);
    close(fd);

    if (mXkbKeymap)
        mXkbState.reset(xkb_state_new(mXkbKeymap.get()));
    else
        mXkbState.reset(nullptr);
}

void Keyboard::keyboard_key(uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    auto code = key + 8; // map to wl_keyboard::keymap_format::keymap_format_xkb_v1

    xkb_keysym_t sym = xkb_state_key_get_one_sym(mXkbState.get(), code);

    auto modifiers = QXkbCommon::modifiers(mXkbState.get());
    int qtkey = QXkbCommon::keysymToQtKey(sym, modifiers, mXkbState.get(), code);
    QString text = QXkbCommon::lookupString(mXkbState.get(), code);

    QEvent::Type type = state == 1 ? QEvent::KeyPress : QEvent::KeyRelease;

    QKeyEvent keyEvent(type, qtkey, modifiers, text, false);
    keyEvent.setAccepted(false);

    if (type == QEvent::KeyPress) {
        Q_EMIT keyPressed(&keyEvent);
    } else {
        Q_EMIT keyReleased(&keyEvent);
    }

    if (!keyEvent.isAccepted()) {
        m_parent->key(serial, time, key, state);
    }
    // see diactrics, and then pretend you haven't seen this
    // TODO don't ship to users like this
    if (keyEvent.timestamp() == -1) {
        m_parent->key(serial, time, key, 0);
    }
}

void Keyboard::keyboard_modifiers(uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    xkb_state_update_mask(mXkbState.get(), mods_depressed, mods_latched, mods_locked, 0, 0, group);

    // currently not filterable
    m_parent->modifiers(serial, mods_depressed, mods_latched, mods_locked, group);
}

#include "moc_inputmethod_p.cpp"
