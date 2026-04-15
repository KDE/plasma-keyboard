/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "feedback.h"

#include "config-plasma-keyboard.h"
#include "plasmakeyboardsettings.h"

#include <QDBusConnection>
#include <QDBusMetaType>
#include <QUrl>

using namespace Qt::StringLiterals;

Feedback::Feedback(QObject *parent)
    : QObject(parent)
{
#if PLASMA_KEYBOARD_VIBRATION_ENABLED
    qDBusRegisterMetaType<VibrationEvent>();
    qDBusRegisterMetaType<VibrationEventList>();
#endif

#if PLASMA_KEYBOARD_SOUNDS_ENABLED
    m_soundEffect.setSource(QUrl(u"qrc:/sounds/keyboard_tick2_quiet.wav"_s));
    m_soundEffect.setVolume(0.6F);
#endif
}

void Feedback::play(Event event)
{
#if PLASMA_KEYBOARD_SOUNDS_ENABLED
    if (PlasmaKeyboardSettings::soundEnabled()) {
        playSound();
    }
#else
    Q_UNUSED(event)
#endif

#if PLASMA_KEYBOARD_VIBRATION_ENABLED
    if (PlasmaKeyboardSettings::vibrationEnabled()) {
        playVibration(event);
    }
#endif
}

#if PLASMA_KEYBOARD_SOUNDS_ENABLED
void Feedback::playSound()
{
    m_soundEffect.stop();
    m_soundEffect.play();
}
#endif

#if PLASMA_KEYBOARD_VIBRATION_ENABLED
void Feedback::playVibration(Event event)
{
    if (!m_hapticInterface) {
        m_hapticInterface = new OrgSigxcpuFeedbackHapticInterface(u"org.sigxcpu.Feedback"_s, u"/org/sigxcpu/Feedback"_s, QDBusConnection::sessionBus(), this);
    }

    int durationMs = PlasmaKeyboardSettings::vibrationMs();
    if (event == SelectionChange) {
        durationMs = qMax(1, durationMs / 2);
    }

    const VibrationEvent vibrationEvent{1.0, static_cast<quint32>(durationMs)};
    const VibrationEventList pattern = {vibrationEvent};
    m_hapticInterface->Vibrate(u"org.kde.plasma.keyboard"_s, pattern);
}
#endif
