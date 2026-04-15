/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "kwinfakeinput.h"

#include "keycodes.h"
#include "qwayland-fake-input.h"
#include "vkbdwaylandinputbackend.h"

#include <QDebug>
#include <QHash>
#include <QSet>
#include <QTimer>
#include <QWaylandClientExtensionTemplate>

#include <linux/input-event-codes.h>

class FakeInput : public QWaylandClientExtensionTemplate<FakeInput>, public QtWayland::org_kde_kwin_fake_input
{
public:
    FakeInput()
        : QWaylandClientExtensionTemplate<FakeInput>(ORG_KDE_KWIN_FAKE_INPUT_KEYBOARD_KEY_SINCE_VERSION)
    {
        initialize();
    }
};

static const QHash<int, int> QT_KEY_TO_LINUX = {
    {Qt::Key_Meta, KEY_LEFTMETA},
    {Qt::Key_Alt, KEY_LEFTALT},
    {Qt::Key_Control, KEY_LEFTCTRL},
    {Qt::Key_Shift, KEY_LEFTSHIFT},
    {Qt::Key_CapsLock, KEY_CAPSLOCK},
    {KeyCodes::KeyFn, KEY_FN},
    {Qt::Key_Tab, KEY_TAB},
    {Qt::Key_Backspace, KEY_BACKSPACE},
    {Qt::Key_Return, KEY_ENTER},
    {Qt::Key_Enter, KEY_ENTER},
    {Qt::Key_Escape, KEY_ESC},
    {Qt::Key_Space, KEY_SPACE},
    {Qt::Key_Left, KEY_LEFT},
    {Qt::Key_Right, KEY_RIGHT},
    {Qt::Key_Up, KEY_UP},
    {Qt::Key_Down, KEY_DOWN},
    {Qt::Key_Insert, KEY_INSERT},
    {Qt::Key_Delete, KEY_DELETE},
    {Qt::Key_Home, KEY_HOME},
    {Qt::Key_End, KEY_END},
    {Qt::Key_PageUp, KEY_PAGEUP},
    {Qt::Key_PageDown, KEY_PAGEDOWN},
    {Qt::Key_A, KEY_A},
    {Qt::Key_B, KEY_B},
    {Qt::Key_C, KEY_C},
    {Qt::Key_D, KEY_D},
    {Qt::Key_E, KEY_E},
    {Qt::Key_F, KEY_F},
    {Qt::Key_G, KEY_G},
    {Qt::Key_H, KEY_H},
    {Qt::Key_I, KEY_I},
    {Qt::Key_J, KEY_J},
    {Qt::Key_K, KEY_K},
    {Qt::Key_L, KEY_L},
    {Qt::Key_M, KEY_M},
    {Qt::Key_N, KEY_N},
    {Qt::Key_O, KEY_O},
    {Qt::Key_P, KEY_P},
    {Qt::Key_Q, KEY_Q},
    {Qt::Key_R, KEY_R},
    {Qt::Key_S, KEY_S},
    {Qt::Key_T, KEY_T},
    {Qt::Key_U, KEY_U},
    {Qt::Key_V, KEY_V},
    {Qt::Key_W, KEY_W},
    {Qt::Key_X, KEY_X},
    {Qt::Key_Y, KEY_Y},
    {Qt::Key_Z, KEY_Z},
    {Qt::Key_0, KEY_0},
    {Qt::Key_1, KEY_1},
    {Qt::Key_2, KEY_2},
    {Qt::Key_3, KEY_3},
    {Qt::Key_4, KEY_4},
    {Qt::Key_5, KEY_5},
    {Qt::Key_6, KEY_6},
    {Qt::Key_7, KEY_7},
    {Qt::Key_8, KEY_8},
    {Qt::Key_9, KEY_9},
    {Qt::Key_F1, KEY_F1},
    {Qt::Key_F2, KEY_F2},
    {Qt::Key_F3, KEY_F3},
    {Qt::Key_F4, KEY_F4},
    {Qt::Key_F5, KEY_F5},
    {Qt::Key_F6, KEY_F6},
    {Qt::Key_F7, KEY_F7},
    {Qt::Key_F8, KEY_F8},
    {Qt::Key_F9, KEY_F9},
    {Qt::Key_F10, KEY_F10},
    {Qt::Key_F11, KEY_F11},
    {Qt::Key_F12, KEY_F12},
    {Qt::Key_Minus, KEY_MINUS},
    {Qt::Key_Equal, KEY_EQUAL},
    {Qt::Key_Backslash, KEY_BACKSLASH},
    {Qt::Key_Slash, KEY_SLASH},
    {Qt::Key_Comma, KEY_COMMA},
    {Qt::Key_Period, KEY_DOT},
    {Qt::Key_Semicolon, KEY_SEMICOLON},
    {Qt::Key_Apostrophe, KEY_APOSTROPHE},
    {Qt::Key_BracketLeft, KEY_LEFTBRACE},
    {Qt::Key_BracketRight, KEY_RIGHTBRACE},
    {Qt::Key_QuoteLeft, KEY_GRAVE},
};

// List of keys to forward through kwin-fake-input rather than input-method-v1
static const QSet<int> DIRECT_FAKE_INPUT_KEYS = {
    Qt::Key_Meta, Qt::Key_Alt,  Qt::Key_Control, Qt::Key_Shift, KeyCodes::KeyFn, Qt::Key_CapsLock, Qt::Key_Tab, Qt::Key_Escape, Qt::Key_Left, Qt::Key_Right,
    Qt::Key_Up,   Qt::Key_Down, Qt::Key_Home,    Qt::Key_End,   Qt::Key_PageUp,  Qt::Key_PageDown, Qt::Key_F1,  Qt::Key_F2,     Qt::Key_F3,   Qt::Key_F4,
    Qt::Key_F5,   Qt::Key_F6,   Qt::Key_F7,      Qt::Key_F8,    Qt::Key_F9,      Qt::Key_F10,      Qt::Key_F11, Qt::Key_F12,
};

// List of keys that are modifiers (and keys entered after are captured as well, even if not in DIRECT_FAKE_INPUT_KEYS)
static const QSet<int> MODIFIER_KEYS = {
    Qt::Key_Meta,
    Qt::Key_Alt,
    Qt::Key_Control,
    Qt::Key_Shift,
    KeyCodes::KeyFn,
};

KWinFakeInput::KWinFakeInput(VkbdWaylandInputBackend *backend, QObject *parent)
    : QObject(parent)
    , m_backend(backend)
{
}

bool KWinFakeInput::shouldUseFakeInput(int key) const
{
    if (DIRECT_FAKE_INPUT_KEYS.contains(key)) {
        return true;
    }
    if (!m_backend || !supportsKey(key)) {
        return false;
    }

    for (const int pressedKey : m_backend->pressedKeys()) {
        if (isModifier(pressedKey)) {
            return true;
        }
    }
    return false;
}

bool KWinFakeInput::sendKeyPressed(int key, bool pressed)
{
    if (!supportsKey(key) || !ensureInitialized()) {
        return false;
    }

    sendFakeKeyboardKey(key, pressed);

    // For a non-modifier key, clear the modifiers
    if (!pressed && !isModifier(key) && m_backend && !m_backend->pressedKeys().isEmpty()) {
        // HACK: clear modifiers after a delay, otherwise it might not be registered by the app
        QTimer::singleShot(20, this, &KWinFakeInput::clearPressedModifiers);
    }
    return true;
}

bool KWinFakeInput::ensureInitialized()
{
    if (m_fakeInput) {
        return true;
    }

    auto *fakeInput = new FakeInput();
    if (!fakeInput->isInitialized() || !fakeInput->isActive()) {
        delete fakeInput;
        return false;
    }

    fakeInput->authenticate({}, {});
    fakeInput->setParent(this);
    m_fakeInput = fakeInput;
    return true;
}

bool KWinFakeInput::isModifier(int key) const
{
    return MODIFIER_KEYS.contains(key);
}

bool KWinFakeInput::supportsKey(int key) const
{
    return QT_KEY_TO_LINUX.contains(key);
}

void KWinFakeInput::clearPressedModifiers()
{
    if (!m_backend) {
        return;
    }

    const QList<int> keys = m_backend->pressedKeys();
    for (const int key : keys) {
        if (isModifier(key)) {
            m_backend->sendKeyPressed(key, false);
        }
    }
}

void KWinFakeInput::sendFakeKeyboardKey(int key, bool pressed)
{
    if (!m_fakeInput) {
        return;
    }

    static_cast<FakeInput *>(m_fakeInput)->keyboard_key(QT_KEY_TO_LINUX.value(key), pressed);
}
