/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "virtualkeyboardcontext.h"

#include "inputbackend.h"
#include "inputengine.h"
#include "keyboardcontroller.h"
#include "keyboardpackageresolver.h"

VirtualKeyboardContext::VirtualKeyboardContext(InputBackend *inputBackend, QObject *parent)
    : QObject(parent)
    , m_inputEngine(new InputEngine(inputBackend, this))
    , m_keyboardPackageResolver(new KeyboardPackageResolver(this))
    , m_keyboardController(new KeyboardController(m_inputEngine, m_keyboardPackageResolver, this))
{
}

InputEngine *VirtualKeyboardContext::inputEngine() const
{
    return m_inputEngine;
}

KeyboardController *VirtualKeyboardContext::keyboardController() const
{
    return m_keyboardController;
}

KeyboardPackageResolver *VirtualKeyboardContext::keyboardPackageResolver() const
{
    return m_keyboardPackageResolver;
}
