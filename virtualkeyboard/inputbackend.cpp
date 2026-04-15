/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "inputbackend.h"

#include <algorithm>

InputBackend::InputBackend(QObject *parent)
    : QObject(parent)
{
}

InputBackend::~InputBackend() = default;

QList<int> InputBackend::pressedKeys() const
{
    return m_pressedKeys;
}

void InputBackend::clearPressedKeys()
{
    const QList<int> keys = m_pressedKeys;
    for (const int key : keys) {
        sendKeyPressed(key, false);
    }
}

void InputBackend::reset()
{
    clearPressedKeys();
    Q_EMIT resetRequested();
}

void InputBackend::setKeyPressed(int key, bool pressed)
{
    if (pressed) {
        if (m_pressedKeys.contains(key)) {
            return;
        }
        m_pressedKeys.append(key);
        std::sort(m_pressedKeys.begin(), m_pressedKeys.end());
    } else {
        if (!m_pressedKeys.removeAll(key)) {
            return;
        }
    }

    Q_EMIT pressedKeysChanged();
}
