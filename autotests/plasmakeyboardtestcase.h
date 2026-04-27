// SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "mockcompositor.h"

#include <QObject>
#include <QProcess>
#include <QString>
#include <QTemporaryDir>
#include <QWindow>
#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandOutput>
#include <QtWaylandCompositor/QWaylandXdgShell>

#include <memory>

/**
 * Base class for plasma-keyboard integration tests.
 *
 * Sets up a mock Wayland compositor with the zwp_input_method_v1 and
 * zwp_input_panel_v1 extensions, launches the plasma-keyboard binary as a
 * child process against it, and waits for the input method to activate and
 * grab the keyboard before any test slot runs.
 *
 * Subclasses override setupConfig() to write whatever KConfig entries their
 * tests require before the child process starts.
 */
class PlasmaKeyboardTestCase : public QObject
{
    Q_OBJECT

protected Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

protected:
    /**
     * Called during initTestCase() after the temporary HOME directory is set up
     * but before the compositor and child process are started.
     *
     * Override to write KConfig entries needed by a specific test suite, e.g.
     * setting the enabled locales.
     */
    virtual void setupConfig()
    {
    }

    /**
     * Sends a key-press event for @p key, waits @p interval milliseconds, then
     * sends the corresponding key-release event. Flushes the Wayland display
     * after each event.
     *
     * @param key  Linux input event code (from linux/input-event-codes.h).
     * @param interval  Time in milliseconds to hold the key down.
     */
    void sendKey(int key, int interval);

    QString m_socketPath;
    std::unique_ptr<QWaylandCompositor> m_compositor;
    std::unique_ptr<InputMethodV1> m_inputMethod;
    std::unique_ptr<InputPanelV1> m_inputPanel;
    std::unique_ptr<QProcess> m_child;

private:
    QTemporaryDir m_runtimeDir;
    QTemporaryDir m_home;
    std::unique_ptr<QWindow> m_outputWindow;
    std::unique_ptr<QWaylandOutput> m_output;
    std::unique_ptr<QWaylandXdgShell> m_xdgShell;
};
