// SPDX-FileCopyrightText: 2025 Milian Wolff <milian.wolff@kdab.com>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

#include <QObject>
#include <qqmlregistration.h>

class WindowMode : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool isFloating MEMBER m_isFloating CONSTANT FINAL)

public:
    WindowMode(bool isFloating = false, QObject *parent = nullptr);
    ~WindowMode() override;

private:
    bool m_isFloating = false;
};
