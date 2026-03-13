/*
 * Copyright (c) 2017 Jan Arne Petersen
 * SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "qwaylandinputpanelsurface_p.h"

#include <QtWaylandClient/private/qwaylandscreen_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(qLcQpaShellIntegration, "qt.qpa.wayland.shell")

QWaylandInputPanelSurface::QWaylandInputPanelSurface(struct ::zwp_input_panel_surface_v1 *object, QtWaylandClient::QWaylandWindow *window)
    : QWaylandShellSurface(window)
    , QtWayland::zwp_input_panel_surface_v1(object)
{
    qCDebug(qLcQpaShellIntegration) << Q_FUNC_INFO;
    window->applyConfigureWhenPossible();
}

QWaylandInputPanelSurface::~QWaylandInputPanelSurface()
{
    qCDebug(qLcQpaShellIntegration) << Q_FUNC_INFO;
    zwp_input_panel_surface_v1_destroy(object());
}

void QWaylandInputPanelSurface::applyConfigure()
{
    const QVariant role = window()->window()->property("plasmaKeyboardInputPanelRole");
    const int roleInt = role.isValid() ? role.toInt() : -1;

    if (roleInt == 1) {
        set_overlay_panel();
    } else if (roleInt == 0) {
        set_toplevel(window()->waylandScreen()->output(), position_center_bottom);
    } else {
        static const bool preferTopLevel = qEnvironmentVariableIntValue("QT_WAYLAND_INPUT_PANEL_TOPLEVEL");
        if (preferTopLevel) {
            set_toplevel(window()->waylandScreen()->output(), position_center_bottom);
        } else {
            set_overlay_panel();
        }
    }

    window()->display()->handleWindowActivated(window());
}

QT_END_NAMESPACE
