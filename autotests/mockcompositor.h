// SPDX-FileCopyrightText: 2026 Aleix Pol <aleixpol@kde.org>
// SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 *
 * Mocks some of the Wayland compositor functionality that plasma-keyboard relies on so
 * that tests can run reliably.
 */

#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandSurface>

#include "qwayland-server-input-method-unstable-v1.h"
#include "qwayland-server-wayland.h"

#include <memory>

/**
 * Server-side mock of the wl_keyboard Wayland object.
 *
 * Sends an XKB keymap to the client on construction, then lets the compositor
 * emit key-press and key-release events via sendKey().
 */
class InputMethodKeyboard : public QObject, public QtWaylandServer::wl_keyboard
{
    Q_OBJECT
public:
    InputMethodKeyboard(wl_client *client, uint32_t id, int version);

    /**
     * Sends a wl_keyboard.enter event for @p surface and stores it as the
     * focus surface for subsequent key events.
     */
    void setFocusSurface(wl_resource *surface);

    /**
     * Sends a wl_keyboard.key event with the given @p key code and @p state
     * (WL_KEYBOARD_KEY_STATE_PRESSED or WL_KEYBOARD_KEY_STATE_RELEASED).
     */
    void sendKey(uint32_t key, uint32_t state);

    /**
     * Returns true once the XKB keymap has been successfully sent to the client.
     */
    bool keymapped() const;

Q_SIGNALS:
    void keymapDone();

private:
    void sendKeymap();

    bool m_keymapped = false;
    uint32_t m_serial = 0;
    QElapsedTimer m_timer;
    wl_resource *m_focusSurface = nullptr;
};

/**
 * Server-side mock of the zwp_input_method_context_v1 Wayland object.
 *
 * Owns an InputMethodKeyboard once the client grabs it, and emits signals
 * for the requests the client sends (commit string, etc.) so tests can spy on
 * them with QSignalSpy.
 */
class InputMethodContext : public QObject, public QtWaylandServer::zwp_input_method_context_v1
{
    Q_OBJECT

public:
    explicit InputMethodContext(wl_resource *focusSurface, QObject *parent = nullptr);

    /**
     * Returns the keyboard grab object once the client has called grab_keyboard,
     * or nullptr before that happens.
     */
    InputMethodKeyboard *keyboard() const;

Q_SIGNALS:
    void keyboardGrabbed();
    void commitStringChanged(const QString &commitString);

protected:
    void zwp_input_method_context_v1_destroy(Resource *resource) override;
    void zwp_input_method_context_v1_commit_string(Resource *resource, uint32_t serial, const QString &text) override;
    void zwp_input_method_context_v1_preedit_string(Resource *resource, uint32_t serial, const QString &text, const QString &commit) override;
    void zwp_input_method_context_v1_delete_surrounding_text(Resource *resource, int32_t index, uint32_t length) override;
    void zwp_input_method_context_v1_keysym(Resource *resource, uint32_t serial, uint32_t time, uint32_t sym, uint32_t state, uint32_t modifiers) override;
    void zwp_input_method_context_v1_grab_keyboard(Resource *resource, uint32_t keyboard) override;

private:
    std::unique_ptr<InputMethodKeyboard> m_keyboard;
    wl_resource *m_focusSurface = nullptr;
};

/**
 * Server-side mock of the zwp_input_panel_surface_v1 Wayland object.
 *
 * Emits toplevelRequested() or overlayRequested() when the client calls the
 * corresponding set_* requests.
 */
class InputPanelSurface : public QObject, public QtWaylandServer::zwp_input_panel_surface_v1
{
    Q_OBJECT

public:
    InputPanelSurface(QWaylandSurface *surface, wl_client *client, uint32_t id, int version, QObject *parent = nullptr);

Q_SIGNALS:
    void toplevelRequested();
    void overlayRequested();

protected:
    void zwp_input_panel_surface_v1_set_toplevel(Resource *resource, wl_resource *output, uint32_t position) override;
    void zwp_input_panel_surface_v1_set_overlay_panel(Resource *resource) override;
    void zwp_input_panel_surface_v1_destroy_resource(Resource *resource) override;

private:
    QWaylandSurface *m_surface = nullptr;
};

/**
 * Compositor extension implementing the zwp_input_panel_v1 protocol.
 *
 * Creates InputPanelSurface objects on demand and tracks how many times
 * the client has requested toplevel or overlay placement.
 */
class InputPanelV1 : public QWaylandCompositorExtensionTemplate<InputPanelV1>, public QtWaylandServer::zwp_input_panel_v1
{
    Q_OBJECT

public:
    explicit InputPanelV1(QWaylandCompositor *compositor);

    void initialize() override;

    /**
     * Returns the number of times overlay placement has been requested across
     * all panel surfaces since this extension was created.
     */
    int overlayPanelCount() const;

    /**
     * Returns the number of times toplevel placement has been requested across
     * all panel surfaces since this extension was created.
     */
    int toplevelPanelCount() const;

    /**
     * Returns the wl_resource for the most recently created input panel surface,
     * or nullptr if none has been created yet.
     */
    wl_resource *lastSurfaceResource() const;

Q_SIGNALS:
    void inputPanelSurfaceCreated();
    void overlayPanelRequested();
    void toplevelPanelRequested();

protected:
    void zwp_input_panel_v1_get_input_panel_surface(Resource *resource, uint32_t id, wl_resource *surface) override;

private:
    int m_overlayPanelCount = 0;
    int m_toplevelPanelCount = 0;
    wl_resource *m_lastSurfaceResource = nullptr;
};

/**
 * Compositor extension implementing the zwp_input_method_v1 protocol.
 *
 * Automatically activates the input method shortly after the client binds to
 * it (configurable via m_autoActivate / m_activateDelayMs), and owns the
 * active InputMethodContext for the lifetime of a session.
 */
class InputMethodV1 : public QWaylandCompositorExtensionTemplate<InputMethodV1>, public QtWaylandServer::zwp_input_method_v1
{
    Q_OBJECT

public:
    explicit InputMethodV1(QWaylandCompositor *compositor);

    void setInputPanel(InputPanelV1 *inputPanel);

    void initialize() override;

    /**
     * Sends an activate event to all bound clients, creating a new context.
     * Does nothing if a context is already active.
     */
    void sendActivate();

    /**
     * Sends a deactivate event to all bound clients and destroys the context.
     * Does nothing if no context is active.
     */
    void sendDeactivate();

    /**
     * Returns the currently active input method context, or nullptr if
     * no activate has been sent yet.
     */
    InputMethodContext *context() const;

Q_SIGNALS:
    void activated();

protected:
    void zwp_input_method_v1_bind_resource(Resource *resource) override;

private:
    const bool m_autoActivate = true;
    const int m_activateDelayMs = 200;

    bool m_activatePending = false;
    std::unique_ptr<InputMethodContext> m_context;
    InputPanelV1 *m_inputPanel = nullptr;
};
