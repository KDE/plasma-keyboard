/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "inputpanelintegration.h"

#include "qwaylandinputpanelshellintegration_p.h"

#include <QDebug>
#include <QWindow>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

bool initInputPanelIntegration(QWindow *window, InputPanelRole::Role role)
{
    if (!window) {
        return false;
    }

    window->setProperty("plasmaKeyboardInputPanelRole", static_cast<int>(role));

    window->create();
    auto waylandWindow = dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle());
    if (!waylandWindow) {
        qWarning() << window << "is not a wayland window. Not creating input panel surface.";
        return false;
    }

    static QWaylandInputPanelShellIntegration *shellIntegration = nullptr;
    if (!shellIntegration) {
        shellIntegration = new QWaylandInputPanelShellIntegration();
        if (!shellIntegration->initialize(waylandWindow->display())) {
            delete shellIntegration;
            shellIntegration = nullptr;
            qWarning() << "Failed to initialize input panel shell integration, possibly because compositor does not support the input_method_v1 protocol or"
                          " because it needs more privileges.";
            return false;
        }
    }

    waylandWindow->setShellIntegration(shellIntegration);
    return true;
}
