/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>

class VkbdWaylandInputBackend;

/**
 * Sends keys unsupported by the input method protocol through KWin's fake input protocol.
 *
 * Keys sent here effectively emulate the keyboard.
 */
class KWinFakeInput : public QObject
{
    Q_OBJECT

public:
    explicit KWinFakeInput(VkbdWaylandInputBackend *backend, QObject *parent = nullptr);

    bool shouldUseFakeInput(int key) const;
    bool sendKeyPressed(int key, bool pressed);

private:
    bool ensureInitialized();
    bool isModifier(int key) const;
    bool supportsKey(int key) const;
    void clearPressedModifiers();
    void sendFakeKeyboardKey(int key, bool pressed);

    QObject *m_fakeInput = nullptr;
    VkbdWaylandInputBackend *m_backend = nullptr;
};
