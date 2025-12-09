/*
 * Copyright (c) 2017 Jan Arne Petersen
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "qwaylandinputpanelshellintegration_p.h"
#include "qwaylandinputpanelsurface_p.h"

#include <QtWaylandClient/private/qwaylandwindow_p.h>

QT_BEGIN_NAMESPACE

QWaylandInputPanelShellIntegration::QWaylandInputPanelShellIntegration()
    : QWaylandShellIntegrationTemplate<QWaylandInputPanelShellIntegration>(1)

{
}

QWaylandInputPanelShellIntegration::~QWaylandInputPanelShellIntegration()
{
}

QtWaylandClient::QWaylandShellSurface *QWaylandInputPanelShellIntegration::createShellSurface(QtWaylandClient::QWaylandWindow *window)
{
    if (!isActive()) {
        return nullptr;
    }
    struct zwp_input_panel_surface_v1 *ip_surface = get_input_panel_surface(window->wlSurface());

    return new QWaylandInputPanelSurface(ip_surface, window);
}

QT_END_NAMESPACE
