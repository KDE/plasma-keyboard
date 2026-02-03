/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

class QWindow;

#include "inputpanelrole.h"

/**
 * @file inputpanelintegration.h
 *
 * Helpers for integrating a QWindow with the compositor as a Wayland input panel
 * (e.g. the on-screen keyboard window or an overlay panel like the diacritics popup).
 */

/**
 * Initialize the given window as an input panel for the compositor.
 *
 * @param window The window to integrate as an input panel. Must be a valid QWindow.
 * @param role The intended compositor role for the window.
 * @return True if integration was initialized successfully, otherwise false.
 */
bool initInputPanelIntegration(QWindow *window, InputPanelRole::Role role);
