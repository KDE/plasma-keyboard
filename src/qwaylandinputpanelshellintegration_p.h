/*
 * Copyright (c) 2017 Jan Arne Petersen
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#ifndef QWAYLANDINPUTPANELSHELLINTEGRATION_H
#define QWAYLANDINPUTPANELSHELLINTEGRATION_H

#include <QtWaylandClient/private/qwaylandshellintegration_p.h>

#include "qwayland-input-method-unstable-v1.h"

QT_BEGIN_NAMESPACE

class QWaylandInputPanelShellIntegration : public QtWaylandClient::QWaylandShellIntegrationTemplate<QWaylandInputPanelShellIntegration>,
                                           public QtWayland::zwp_input_panel_v1
{
public:
    QWaylandInputPanelShellIntegration();
    ~QWaylandInputPanelShellIntegration() override;

    QtWaylandClient::QWaylandShellSurface *createShellSurface(QtWaylandClient::QWaylandWindow *window) override;
};

QT_END_NAMESPACE

#endif // QWAYLANDINPUTPANELSHELLINTEGRATION_H
