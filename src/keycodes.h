/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <qqmlintegration.h>

class KeyCodes : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    using QObject::QObject;

    enum Key {
        // HACK: Qt does not have a Key_Fn, so invent our own constant for it
        KeyFn = 0x01800000,
    };
    Q_ENUM(Key)
};
