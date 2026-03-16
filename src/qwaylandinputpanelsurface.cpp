/*
 * Copyright (c) 2017 Jan Arne Petersen
 * SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "inputpanelrole.h"
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
    const QVariant roleVariant = window()->window()->property("plasmaKeyboardInputPanelRole");
    const int role = roleVariant.isValid() ? roleVariant.toInt() : -1;

    switch (role) {
    case InputPanelRole::OverlayPanel:
        set_overlay_panel();
        break;
    case InputPanelRole::Keyboard:
    default:
        set_toplevel(window()->waylandScreen()->output(), position_center_bottom);
        break;
    }

    window()->display()->handleWindowActivated(window());
}

QT_END_NAMESPACE
