/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <qqmlintegration.h>

class InputBackend;
class InputEngine;
class KeyboardController;
class KeyboardPackageResolver;

class VirtualKeyboardContext : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
    Q_PROPERTY(InputEngine *inputEngine READ inputEngine CONSTANT)
    Q_PROPERTY(KeyboardController *keyboardController READ keyboardController CONSTANT)
    Q_PROPERTY(KeyboardPackageResolver *keyboardPackageResolver READ keyboardPackageResolver CONSTANT)

public:
    explicit VirtualKeyboardContext(InputBackend *inputBackend, QObject *parent = nullptr);

    InputEngine *inputEngine() const;
    KeyboardController *keyboardController() const;
    KeyboardPackageResolver *keyboardPackageResolver() const;

private:
    InputEngine *m_inputEngine = nullptr;
    KeyboardPackageResolver *m_keyboardPackageResolver = nullptr;
    KeyboardController *m_keyboardController = nullptr;
};
