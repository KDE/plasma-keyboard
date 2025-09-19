// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "vibration.h"

Vibration::Vibration(QObject *parent)
    : QObject{parent}
{
    qDBusRegisterMetaType<VibrationEvent>();
    qDBusRegisterMetaType<VibrationEventList>();
}

void Vibration::vibrate(int durationMs)
{
    // Only create interface when needed.
    if (!m_interface) {
        const auto objectPath = QStringLiteral("/org/sigxcpu/Feedback");
        m_interface = new OrgSigxcpuFeedbackHapticInterface(QStringLiteral("org.sigxcpu.Feedback"), objectPath, QDBusConnection::sessionBus(), this);
    }

    const QString appId = QStringLiteral("org.kde.plasma.keyboard");
    const VibrationEvent event{1.0, static_cast<quint32>(durationMs)};
    const VibrationEventList pattern = {event};

    m_interface->Vibrate(appId, pattern);
}

#include "moc_vibration.cpp"
