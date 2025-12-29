/*
    SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

class QWindow;

enum class InputPanelRole {
    Keyboard = 0,
    OverlayPanel = 1,
};

bool initInputPanelIntegration(QWindow *window, InputPanelRole role);
