/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "plasmakeyboardstate.h"

PlasmaKeyboardState *PlasmaKeyboardState::self()
{
    static PlasmaKeyboardState *singleton = new PlasmaKeyboardState();
    return singleton;
}

bool PlasmaKeyboardState::modifierMetaPressed() const
{
    return m_modifierMetaPressed;
}

void PlasmaKeyboardState::setModifierMetaPressed(bool pressed)
{
    if (pressed == m_modifierMetaPressed) {
        return;
    }

    m_modifierMetaPressed = pressed;
    Q_EMIT modifierMetaPressedChanged();
}

bool PlasmaKeyboardState::modifierCapsLockPressed() const
{
    return m_modifierCapsLockPressed;
}

void PlasmaKeyboardState::setModifierCapsLockPressed(bool pressed)
{
    if (pressed == m_modifierCapsLockPressed) {
        return;
    }

    m_modifierCapsLockPressed = pressed;
    Q_EMIT modifierCapsLockPressedChanged();
}

bool PlasmaKeyboardState::modifierControlPressed() const
{
    return m_modifierControlPressed;
}

void PlasmaKeyboardState::setModifierControlPressed(bool pressed)
{
    if (pressed == m_modifierControlPressed) {
        return;
    }

    m_modifierControlPressed = pressed;
    Q_EMIT modifierControlPressedChanged();
}

bool PlasmaKeyboardState::modifierAltPressed() const
{
    return m_modifierAltPressed;
}

void PlasmaKeyboardState::setModifierAltPressed(bool pressed)
{
    if (pressed == m_modifierAltPressed) {
        return;
    }

    m_modifierAltPressed = pressed;
    Q_EMIT modifierAltPressedChanged();
}

bool PlasmaKeyboardState::modifierShiftPressed() const
{
    return m_modifierShiftPressed;
}

void PlasmaKeyboardState::setModifierShiftPressed(bool pressed)
{
    if (pressed == m_modifierShiftPressed) {
        return;
    }

    m_modifierShiftPressed = pressed;
    Q_EMIT modifierShiftPressedChanged();
}
