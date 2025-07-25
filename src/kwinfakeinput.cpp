/*
    SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
    SPDX-FileCopyrightText: 2022 Aleix Pol Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "kwinfakeinput.h"
#include "qwayland-fake-input.h"
#include <QWaylandClientExtensionTemplate>
#include <KLocalizedString>
#include <QDebug>
#include <QtGlobal>
#include <QTimer>

#include <linux/input-event-codes.h>

#include "plasmakeyboardstate.h"

// Map Qt keys to Linux keys (linux/input-event-codes.h)
static const QMap<int, int> QT_KEY_TO_LINUX = {
    {Qt::Key_Meta,        KEY_LEFTMETA},
    {Qt::Key_Alt,         KEY_LEFTALT},
    {Qt::Key_Control,     KEY_LEFTCTRL},
    {Qt::Key_Shift,       KEY_LEFTSHIFT},
    {Qt::Key_CapsLock,    KEY_CAPSLOCK},

    {Qt::Key_Tab,         KEY_TAB},
    {Qt::Key_Backspace,   KEY_BACKSPACE},
    {Qt::Key_Return,      KEY_ENTER},
    {Qt::Key_Enter,       KEY_ENTER},
    {Qt::Key_Escape,      KEY_ESC},
    {Qt::Key_Space,       KEY_SPACE},
    {Qt::Key_Left,        KEY_LEFT},
    {Qt::Key_Right,       KEY_RIGHT},
    {Qt::Key_Up,          KEY_UP},
    {Qt::Key_Down,        KEY_DOWN},
    {Qt::Key_Insert,      KEY_INSERT},
    {Qt::Key_Delete,      KEY_DELETE},
    {Qt::Key_Home,        KEY_HOME},
    {Qt::Key_End,         KEY_END},
    {Qt::Key_PageUp,      KEY_PAGEUP},
    {Qt::Key_PageDown,    KEY_PAGEDOWN},

    {Qt::Key_F1,          KEY_F1},
    {Qt::Key_F2,          KEY_F2},
    {Qt::Key_F3,          KEY_F3},
    {Qt::Key_F4,          KEY_F4},
    {Qt::Key_F5,          KEY_F5},
    {Qt::Key_F6,          KEY_F6},
    {Qt::Key_F7,          KEY_F7},
    {Qt::Key_F8,          KEY_F8},
    {Qt::Key_F9,          KEY_F9},
    {Qt::Key_F10,         KEY_F10},
    {Qt::Key_F11,         KEY_F11},
    {Qt::Key_F12,         KEY_F12},

    {Qt::Key_A,           KEY_A},
    {Qt::Key_B,           KEY_B},
    {Qt::Key_C,           KEY_C},
    {Qt::Key_D,           KEY_D},
    {Qt::Key_E,           KEY_E},
    {Qt::Key_F,           KEY_F},
    {Qt::Key_G,           KEY_G},
    {Qt::Key_H,           KEY_H},
    {Qt::Key_I,           KEY_I},
    {Qt::Key_J,           KEY_J},
    {Qt::Key_K,           KEY_K},
    {Qt::Key_L,           KEY_L},
    {Qt::Key_M,           KEY_M},
    {Qt::Key_N,           KEY_N},
    {Qt::Key_O,           KEY_O},
    {Qt::Key_P,           KEY_P},
    {Qt::Key_Q,           KEY_Q},
    {Qt::Key_R,           KEY_R},
    {Qt::Key_S,           KEY_S},
    {Qt::Key_T,           KEY_T},
    {Qt::Key_U,           KEY_U},
    {Qt::Key_V,           KEY_V},
    {Qt::Key_W,           KEY_W},
    {Qt::Key_X,           KEY_X},
    {Qt::Key_Y,           KEY_Y},
    {Qt::Key_Z,           KEY_Z},

    {Qt::Key_0,           KEY_0},
    {Qt::Key_1,           KEY_1},
    {Qt::Key_2,           KEY_2},
    {Qt::Key_3,           KEY_3},
    {Qt::Key_4,           KEY_4},
    {Qt::Key_5,           KEY_5},
    {Qt::Key_6,           KEY_6},
    {Qt::Key_7,           KEY_7},
    {Qt::Key_8,           KEY_8},
    {Qt::Key_9,           KEY_9},

    {Qt::Key_Minus,       KEY_MINUS},
    {Qt::Key_Equal,       KEY_EQUAL},
    {Qt::Key_Backslash,   KEY_BACKSLASH},
    {Qt::Key_Slash,       KEY_SLASH},
    {Qt::Key_Comma,       KEY_COMMA},
    {Qt::Key_Period,      KEY_DOT},
    {Qt::Key_Semicolon,   KEY_SEMICOLON},
    {Qt::Key_Apostrophe,  KEY_APOSTROPHE},
    {Qt::Key_BracketLeft, KEY_LEFTBRACE},
    {Qt::Key_BracketRight,KEY_RIGHTBRACE},
    {Qt::Key_QuoteLeft,   KEY_GRAVE},
};


// Keys to send through the "fake input" protocol
static const std::set<int> FAKE_INPUT_KEYS = {
    Qt::Key_Meta,
    Qt::Key_Alt,
    Qt::Key_Control,
    Qt::Key_CapsLock,
    Qt::Key_Shift,
    Qt::Key_Tab,
    Qt::Key_Escape,
    Qt::Key_Up,
    Qt::Key_Down,
    Qt::Key_Left,
    Qt::Key_Right
};

// Keys that are modifiers, and so have a "toggle" state
static const std::set<int> MODIFIER_KEYS = {
    Qt::Key_Meta,
    Qt::Key_Alt,
    Qt::Key_Control,
    Qt::Key_CapsLock,
    Qt::Key_Shift
};

class FakeInput : public QWaylandClientExtensionTemplate<FakeInput>, public QtWayland::org_kde_kwin_fake_input
{
public:
    FakeInput()
        : QWaylandClientExtensionTemplate<FakeInput>(ORG_KDE_KWIN_FAKE_INPUT_KEYBOARD_KEY_SINCE_VERSION)
    {
        initialize();
    }
};

bool KWinFakeInput::init()
{
    m_ext = new FakeInput;
    if (!m_ext->isInitialized()) {
        qWarning() << "Could not initialise the org_kde_kwin_fake_input implementation";
        return false;
    }
    m_ext->setParent(this);
    if (!m_ext->isActive()) {
        return false;
    }
    m_ext->authenticate({}, {});
    return true;
}

bool KWinFakeInput::shouldUseFakeInput(int key) const
{
    return FAKE_INPUT_KEYS.find(key) != FAKE_INPUT_KEYS.end();
}

bool KWinFakeInput::isModifier(int key) const
{
    return MODIFIER_KEYS.find(key) != MODIFIER_KEYS.end();
}

bool KWinFakeInput::isModifierPressed() const
{
    return !m_toggledModifierKeys.empty();
}

void KWinFakeInput::pressModifier(int modifierKey)
{
    if (!QT_KEY_TO_LINUX.contains(modifierKey)) {
        qWarning() << "Cannot find Qt key -> Linux event code mapping for" << modifierKey;
        return;
    }

    int linuxKey = QT_KEY_TO_LINUX[modifierKey];

    // Toggle modifier state
    if (m_toggledModifierKeys.contains(linuxKey)) {
        m_toggledModifierKeys.remove(linuxKey);
    } else {
        m_toggledModifierKeys.insert(linuxKey);
    }
    updateModifierStateToUI();

    sendFakeKeyboardKey(linuxKey, m_toggledModifierKeys.contains(linuxKey));
}

void KWinFakeInput::pressKey(int key, bool pressed)
{
    if (!QT_KEY_TO_LINUX.contains(key)) {
        qWarning() << "Cannot find Qt key -> Linux event code mapping for" << key;
        return;
    }

    int linuxKey = QT_KEY_TO_LINUX[key];
    sendFakeKeyboardKey(linuxKey, pressed);

    // Release any pressed modifiers
    if (!pressed) {
        // HACK: we can't release modifiers at the exact same time as the key, apps like Chromium might ignore the event (ex. Ctrl+A select all)
        // Use a slight delay
        QTimer::singleShot(20, [this]() {
            releaseModifiers();
        });
    }
}

void KWinFakeInput::releaseModifiers()
{
    // Release pressed modifiers
    for (int modifier : m_toggledModifierKeys) {
        sendFakeKeyboardKey(modifier, false); // Unpress the modifier
    }
    m_toggledModifierKeys.clear();
    updateModifierStateToUI();
}

void KWinFakeInput::sendFakeKeyboardKey(int key, bool pressed)
{
    m_ext->keyboard_key(key, pressed);
}

void KWinFakeInput::updateModifierStateToUI()
{
    PlasmaKeyboardState::self()->setModifierMetaPressed(m_toggledModifierKeys.contains(QT_KEY_TO_LINUX[Qt::Key_Meta]));
    PlasmaKeyboardState::self()->setModifierCapsLockPressed(m_toggledModifierKeys.contains(QT_KEY_TO_LINUX[Qt::Key_CapsLock]));
    PlasmaKeyboardState::self()->setModifierControlPressed(m_toggledModifierKeys.contains(QT_KEY_TO_LINUX[Qt::Key_Control]));
    PlasmaKeyboardState::self()->setModifierAltPressed(m_toggledModifierKeys.contains(QT_KEY_TO_LINUX[Qt::Key_Alt]));
    PlasmaKeyboardState::self()->setModifierShiftPressed(m_toggledModifierKeys.contains(QT_KEY_TO_LINUX[Qt::Key_Shift]));
}

#include "moc_kwinfakeinput.cpp"
