// SPDX-FileCopyrightText: 2026 Aleix Pol <aleixpol@kde.org>
// SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "mockcompositor.h"

#include <QTimer>
#include <QtWaylandCompositor/QWaylandCompositor>

#include <fcntl.h>
#include <private/qxkbcommon_p.h>
#include <sys/mman.h>
#include <unistd.h>
#include <xkbcommon/xkbcommon.h>

static int createAnonymousKeymapFile(off_t size)
{
    int fd = -1;
#ifdef MFD_CLOEXEC
    fd = memfd_create("plasma-keyboard-keymap", MFD_CLOEXEC);
    if (fd >= 0) {
        if (ftruncate(fd, size) != 0) {
            close(fd);
            return -1;
        }
        return fd;
    }
#endif
    char name[] = "/tmp/plasma-keyboard-keymap-XXXXXX";
    fd = mkstemp(name);
    if (fd < 0) {
        return -1;
    }
    unlink(name);
    if (ftruncate(fd, size) != 0) {
        close(fd);
        return -1;
    }
    return fd;
}

// InputMethodKeyboard

InputMethodKeyboard::InputMethodKeyboard(wl_client *client, uint32_t id, int version)
    : QtWaylandServer::wl_keyboard(client, id, version)
{
    sendKeymap();
    m_timer.start();
}

void InputMethodKeyboard::setFocusSurface(wl_resource *surface)
{
    if (!surface || m_focusSurface == surface) {
        return;
    }
    m_focusSurface = surface;
    send_enter(++m_serial, m_focusSurface, QByteArray());
}

void InputMethodKeyboard::sendKey(uint32_t key, uint32_t state)
{
    send_key(++m_serial, static_cast<uint32_t>(m_timer.elapsed()), key, state);
}

bool InputMethodKeyboard::keymapped() const
{
    return m_keymapped;
}

void InputMethodKeyboard::sendKeymap()
{
    QXkbCommon::ScopedXKBContext context(xkb_context_new(XKB_CONTEXT_NO_FLAGS));
    if (!context) {
        qWarning() << "Failed to create xkb context";
        return;
    }

    xkb_rule_names names = {};
    names.rules = "evdev";
    names.layout = "us";

    QXkbCommon::ScopedXKBKeymap keymap(xkb_keymap_new_from_names(context.get(), &names, XKB_KEYMAP_COMPILE_NO_FLAGS));
    if (!keymap) {
        qWarning() << "Failed to create xkb keymap";
        return;
    }

    char *mapStr = xkb_keymap_get_as_string(keymap.get(), XKB_KEYMAP_FORMAT_TEXT_V1);
    if (!mapStr) {
        qWarning() << "Failed to get xkb keymap string";
        return;
    }

    const QByteArray mapData(mapStr);
    free(mapStr);

    const int fd = createAnonymousKeymapFile(mapData.size() + 1);
    if (fd < 0) {
        qWarning() << "Failed to create keymap file";
        return;
    }

    if (write(fd, mapData.constData(), mapData.size()) != mapData.size()) {
        close(fd);
        qWarning() << "Failed to write keymap";
        return;
    }
    if (write(fd, "\0", 1) != 1) {
        close(fd);
        qWarning() << "Failed to write keymap terminator";
        return;
    }
    lseek(fd, 0, SEEK_SET);

    send_keymap(WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, fd, mapData.size() + 1);
    close(fd);
    m_keymapped = true;
    Q_EMIT keymapDone();
}

// InputMethodContext

InputMethodContext::InputMethodContext(wl_resource *focusSurface, QObject *parent)
    : QObject(parent)
    , m_focusSurface(focusSurface)
{
}

InputMethodKeyboard *InputMethodContext::keyboard() const
{
    return m_keyboard.get();
}

void InputMethodContext::zwp_input_method_context_v1_destroy(Resource *resource)
{
    wl_resource_destroy(resource->handle);
}

void InputMethodContext::zwp_input_method_context_v1_commit_string(Resource *resource, uint32_t serial, const QString &text)
{
    Q_UNUSED(resource);
    qInfo().noquote() << "commit_string" << serial << text;
    Q_EMIT commitStringChanged(text);
}

void InputMethodContext::zwp_input_method_context_v1_preedit_string(Resource *resource, uint32_t serial, const QString &text, const QString &commit)
{
    Q_UNUSED(resource);
    qInfo().noquote() << "preedit_string" << serial << text << commit;
}

void InputMethodContext::zwp_input_method_context_v1_delete_surrounding_text(Resource *resource, int32_t index, uint32_t length)
{
    Q_UNUSED(resource);
    qInfo() << "delete_surrounding_text" << index << length;
}

void InputMethodContext::zwp_input_method_context_v1_keysym(Resource *resource,
                                                            uint32_t serial,
                                                            uint32_t time,
                                                            uint32_t sym,
                                                            uint32_t state,
                                                            uint32_t modifiers)
{
    Q_UNUSED(resource);
    qInfo() << "keysym" << serial << time << sym << state << modifiers;
}

void InputMethodContext::zwp_input_method_context_v1_grab_keyboard(Resource *resource, uint32_t keyboard)
{
    m_keyboard = std::make_unique<InputMethodKeyboard>(resource->client(), keyboard, resource->version());
    m_keyboard->setFocusSurface(m_focusSurface);
    Q_EMIT keyboardGrabbed();
    qInfo() << "input_method_context grab_keyboard";
}

// InputPanelSurface

InputPanelSurface::InputPanelSurface(QWaylandSurface *surface, wl_client *client, uint32_t id, int version, QObject *parent)
    : QObject(parent)
    , QtWaylandServer::zwp_input_panel_surface_v1(client, id, version)
    , m_surface(surface)
{
}

void InputPanelSurface::zwp_input_panel_surface_v1_set_toplevel(Resource *resource, wl_resource *output, uint32_t position)
{
    Q_UNUSED(resource);
    Q_UNUSED(output);
    Q_UNUSED(position);
    Q_EMIT toplevelRequested();
    qInfo() << "input_panel_surface set_toplevel";
}

void InputPanelSurface::zwp_input_panel_surface_v1_set_overlay_panel(Resource *resource)
{
    Q_UNUSED(resource);
    Q_EMIT overlayRequested();
    qInfo() << "input_panel_surface set_overlay_panel";
}

void InputPanelSurface::zwp_input_panel_surface_v1_destroy_resource(Resource *resource)
{
    Q_UNUSED(resource);
    delete this;
}

// InputPanelV1

InputPanelV1::InputPanelV1(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<InputPanelV1>(compositor)
{
}

void InputPanelV1::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    auto *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qWarning() << "No compositor available when initializing input panel";
        return;
    }
    init(compositor->display(), interfaceVersion());
}

int InputPanelV1::overlayPanelCount() const
{
    return m_overlayPanelCount;
}

int InputPanelV1::toplevelPanelCount() const
{
    return m_toplevelPanelCount;
}

wl_resource *InputPanelV1::lastSurfaceResource() const
{
    return m_lastSurfaceResource;
}

void InputPanelV1::zwp_input_panel_v1_get_input_panel_surface(Resource *resource, uint32_t id, wl_resource *surface)
{
    auto *wlSurface = QWaylandSurface::fromResource(surface);
    if (!wlSurface) {
        qWarning() << "input_panel_surface requested for unknown surface";
        return;
    }

    m_lastSurfaceResource = surface;
    auto *panelSurface = new InputPanelSurface(wlSurface, resource->client(), id, resource->version(), this);
    connect(panelSurface, &InputPanelSurface::toplevelRequested, this, [this] {
        ++m_toplevelPanelCount;
        Q_EMIT toplevelPanelRequested();
    });
    connect(panelSurface, &InputPanelSurface::overlayRequested, this, [this] {
        ++m_overlayPanelCount;
        Q_EMIT overlayPanelRequested();
    });
    Q_EMIT inputPanelSurfaceCreated();
    qInfo() << "input_panel_surface created";
}

// InputMethodV1

InputMethodV1::InputMethodV1(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<InputMethodV1>(compositor)
{
}

void InputMethodV1::setInputPanel(InputPanelV1 *inputPanel)
{
    m_inputPanel = inputPanel;
}

void InputMethodV1::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    auto *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qWarning() << "No compositor available when initializing input method";
        return;
    }
    init(compositor->display(), interfaceVersion());
}

void InputMethodV1::sendActivate()
{
    if (m_context) {
        return;
    }

    const wl_resource *focusSurface = m_inputPanel ? m_inputPanel->lastSurfaceResource() : nullptr;
    m_context = std::make_unique<InputMethodContext>(const_cast<wl_resource *>(focusSurface), this);
    for (auto *resource : resourceMap()) {
        auto *contextResource = m_context->add(resource->client(), resource->version());
        send_activate(resource->handle, contextResource->handle);
    }
    qInfo() << "input_method_v1 activated";
    Q_EMIT activated();
}

void InputMethodV1::sendDeactivate()
{
    if (!m_context) {
        return;
    }

    for (auto *resource : resourceMap()) {
        auto *contextResource = m_context->resourceMap().value(resource->client());
        if (contextResource) {
            send_deactivate(resource->handle, contextResource->handle);
        }
    }
    m_context.reset();
    qInfo() << "input_method_v1 deactivated";
}

InputMethodContext *InputMethodV1::context() const
{
    return m_context.get();
}

void InputMethodV1::zwp_input_method_v1_bind_resource(Resource *resource)
{
    if (m_context) {
        auto *contextResource = m_context->add(resource->client(), resource->version());
        send_activate(resource->handle, contextResource->handle);
        return;
    }

    if (!m_autoActivate || m_activatePending) {
        return;
    }

    m_activatePending = true;
    QTimer::singleShot(m_activateDelayMs, this, [this] {
        m_activatePending = false;
        sendActivate();
    });
}

#include "mockcompositor.moc"
