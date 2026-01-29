/*
    SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

class QWindow;

/**
 * @file inputpanelintegration.h
 *
 * Helpers for integrating a QWindow with the compositor as a Wayland input panel
 * (e.g. the on-screen keyboard window or an overlay panel like the diacritics popup).
 */

/**
 * Declares how the window should be exposed to the compositor.
 *
 * The role influences how the compositor treats the surface (placement, focus
 * interactions, and stacking relative to other surfaces).
 */
enum class InputPanelRole {
    /** Main on-screen keyboard window. */
    Keyboard = 0,
    /** Overlay panel associated with and in proximity to a text input field. */
    OverlayPanel = 1,
};

/**
 * Initialize the given window as an input panel for the compositor.
 *
 * @param window The window to integrate as an input panel. Must be a valid QWindow.
 * @param role The intended compositor role for the window.
 * @return True if integration was initialized successfully, otherwise false.
 */
bool initInputPanelIntegration(QWindow *window, InputPanelRole role);
