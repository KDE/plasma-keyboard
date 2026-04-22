/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "config-plasma-keyboard.h"

#include <QObject>
#include <qqmlintegration.h>

#if PLASMA_KEYBOARD_SOUNDS_ENABLED
#include <QSoundEffect>
#endif

#if PLASMA_KEYBOARD_VIBRATION_ENABLED
#include "hapticinterface.h"
#include "vibrationevent.h"
#endif

class Feedback : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    enum Event {
        Press,
        SelectionChange,
        SelectionCommit,
    };
    Q_ENUM(Event)

    explicit Feedback(QObject *parent = nullptr);

    Q_INVOKABLE void play(Event event = Press);

private:
#if PLASMA_KEYBOARD_SOUNDS_ENABLED
    void playSound();
    QSoundEffect m_soundEffect;
#endif

#if PLASMA_KEYBOARD_VIBRATION_ENABLED
    void playVibration(Event event);
    OrgSigxcpuFeedbackHapticInterface *m_hapticInterface = nullptr;
#endif
};
