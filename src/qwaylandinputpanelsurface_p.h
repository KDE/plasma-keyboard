/* 
 * Copyright (c) 2017 Jan Arne Petersen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
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

#endif //QWAYLANDINPUTPANELSURFACE_H
