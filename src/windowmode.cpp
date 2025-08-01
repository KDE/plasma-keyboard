// SPDX-FileCopyrightText: 2025 Milian Wolff <milian.wolff@kdab.com>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "windowmode.h"

WindowMode::WindowMode(bool isFloating, QObject *parent)
    : QObject{parent}
    , m_isFloating(isFloating)
{
}

WindowMode::~WindowMode() = default;
