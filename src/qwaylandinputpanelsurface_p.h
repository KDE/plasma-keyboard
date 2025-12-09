/*
 * Copyright (c) 2017 Jan Arne Petersen
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#ifndef QWAYLANDINPUTPANELSURFACE_H
#define QWAYLANDINPUTPANELSURFACE_H

#include "qwayland-input-method-unstable-v1.h"

#include <QtCore/QLoggingCategory>
#include <QtWaylandClient/private/qwaylandshellsurface_p.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(qLcQpaShellIntegration)

class QWaylandInputPanelSurface : public QtWaylandClient::QWaylandShellSurface, public QtWayland::zwp_input_panel_surface_v1
{
public:
    QWaylandInputPanelSurface(struct ::zwp_input_panel_surface_v1 *object, QtWaylandClient::QWaylandWindow *window);
    ~QWaylandInputPanelSurface() override;

    void applyConfigure() override;
};

QT_END_NAMESPACE

#endif // QWAYLANDINPUTPANELSURFACE_H
