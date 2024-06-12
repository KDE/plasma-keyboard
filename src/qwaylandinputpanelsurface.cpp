#include "qwaylandinputpanelsurface_p.h"

#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <QtWaylandClient/private/qwaylandscreen_p.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(qLcQpaShellIntegration, "qt.qpa.wayland.shell")

QWaylandInputPanelSurface::QWaylandInputPanelSurface(struct ::zwp_input_panel_surface_v1 *object,
                                                     QtWaylandClient::QWaylandWindow *window)
    : QWaylandShellSurface(window)
    , QtWayland::zwp_input_panel_surface_v1(object)
{
    qCDebug(qLcQpaShellIntegration) << Q_FUNC_INFO;
    window->applyConfigureWhenPossible();
}

QWaylandInputPanelSurface::~QWaylandInputPanelSurface()
{
    qCDebug(qLcQpaShellIntegration) << Q_FUNC_INFO;
}

void QWaylandInputPanelSurface::applyConfigure()
{
    static const bool preferTopLevel = qEnvironmentVariableIntValue("QT_WAYLAND_INPUT_PANEL_TOPLEVEL");
    if (preferTopLevel)
        set_toplevel(window()->waylandScreen()->output(), position_center_bottom);
    else
        set_overlay_panel();
}

QT_END_NAMESPACE
