/* 
 * Copyright (c) 2017 Jan Arne Petersen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
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

#endif //QWAYLANDINPUTPANELSHELLINTEGRATION_H
