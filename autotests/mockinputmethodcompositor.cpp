// SPDX-FileCopyrightText: 2026 Aleix Pol <aleixpol@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <KConfig>
#include <KConfigGroup>
#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTimer>
#include <QWindow>
#include <QtTest/QTest>

#include <memory>

#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandCompositorExtension>
#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandOutput>
#include <QtWaylandCompositor/QWaylandOutputMode>
#include <QtWaylandCompositor/QWaylandSurface>
#include <QtWaylandCompositor/QWaylandXdgShell>
#include <private/qxkbcommon_p.h>

#include "mockinputmethodcompositor_config.h"
#include "qwayland-server-input-method-unstable-v1.h"
#include "qwayland-server-wayland.h"

#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <sys/mman.h>
#include <unistd.h>
#include <xkbcommon/xkbcommon.h>

#define PLASMA_KEYBOARD_UNDER_GDB 0

using namespace Qt::StringLiterals;

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

class InputMethodKeyboard : public QObject, public QtWaylandServer::wl_keyboard
{
    Q_OBJECT
public:
    InputMethodKeyboard(wl_client *client, uint32_t id, int version)
        : QtWaylandServer::wl_keyboard(client, id, version)
    {
        sendKeymap();
        m_timer.start();
    }

    void setFocusSurface(wl_resource *surface)
    {
        if (!surface || m_focusSurface == surface) {
            return;
        }
        m_focusSurface = surface;
        send_enter(++m_serial, m_focusSurface, QByteArray());
    }

    void sendKey(uint32_t key, uint32_t state)
    {
        send_key(++m_serial, static_cast<uint32_t>(m_timer.elapsed()), key, state);
    }

    bool keymapped() const
    {
        return m_keymapped;
    }

Q_SIGNALS:
    void keymapDone();

private:
    void sendKeymap()
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

    bool m_keymapped = false;
    uint32_t m_serial = 0;
    QElapsedTimer m_timer;
    wl_resource *m_focusSurface = nullptr;
};

class InputMethodContext : public QObject, public QtWaylandServer::zwp_input_method_context_v1
{
    Q_OBJECT

public:
    explicit InputMethodContext(wl_resource *focusSurface, QObject *parent = nullptr)
        : QObject(parent)
        , m_focusSurface(focusSurface)
    {
    }

    InputMethodKeyboard *keyboard() const
    {
        return m_keyboard.get();
    }

Q_SIGNALS:
    void keyboardGrabbed();
    void commitStringChanged(const QString &commitString);

protected:
    void zwp_input_method_context_v1_destroy(Resource *resource) override
    {
        wl_resource_destroy(resource->handle);
    }

    void zwp_input_method_context_v1_commit_string(Resource *resource, uint32_t serial, const QString &text) override
    {
        Q_UNUSED(resource);
        qInfo().noquote() << "commit_string" << serial << text;
        Q_EMIT commitStringChanged(text);
    }

    void zwp_input_method_context_v1_preedit_string(Resource *resource, uint32_t serial, const QString &text, const QString &commit) override
    {
        Q_UNUSED(resource);
        qInfo().noquote() << "preedit_string" << serial << text << commit;
    }

    void zwp_input_method_context_v1_delete_surrounding_text(Resource *resource, int32_t index, uint32_t length) override
    {
        Q_UNUSED(resource);
        qInfo() << "delete_surrounding_text" << index << length;
    }

    void zwp_input_method_context_v1_keysym(Resource *resource, uint32_t serial, uint32_t time, uint32_t sym, uint32_t state, uint32_t modifiers) override
    {
        Q_UNUSED(resource);
        qInfo() << "keysym" << serial << time << sym << state << modifiers;
    }

    void zwp_input_method_context_v1_grab_keyboard(Resource *resource, uint32_t keyboard) override
    {
        m_keyboard = std::make_unique<InputMethodKeyboard>(resource->client(), keyboard, resource->version());
        m_keyboard->setFocusSurface(m_focusSurface);
        Q_EMIT keyboardGrabbed();
        qInfo() << "input_method_context grab_keyboard";
    }

private:
    std::unique_ptr<InputMethodKeyboard> m_keyboard;
    wl_resource *m_focusSurface = nullptr;
};

class InputPanelSurface : public QObject, public QtWaylandServer::zwp_input_panel_surface_v1
{
    Q_OBJECT

public:
    InputPanelSurface(QWaylandSurface *surface, wl_client *client, uint32_t id, int version, QObject *parent = nullptr)
        : QObject(parent)
        , QtWaylandServer::zwp_input_panel_surface_v1(client, id, version)
        , m_surface(surface)
    {
    }

Q_SIGNALS:
    void toplevelRequested();
    void overlayRequested();

protected:
    void zwp_input_panel_surface_v1_set_toplevel(Resource *resource, wl_resource *output, uint32_t position) override
    {
        Q_UNUSED(resource);
        Q_UNUSED(output);
        Q_UNUSED(position);
        Q_EMIT toplevelRequested();
        qInfo() << "input_panel_surface set_toplevel";
    }

    void zwp_input_panel_surface_v1_set_overlay_panel(Resource *resource) override
    {
        Q_UNUSED(resource);
        Q_EMIT overlayRequested();
        qInfo() << "input_panel_surface set_overlay_panel";
    }

    void zwp_input_panel_surface_v1_destroy_resource(Resource *resource) override
    {
        Q_UNUSED(resource);
        delete this;
    }

private:
    QWaylandSurface *m_surface = nullptr;
};

class InputPanelV1 : public QWaylandCompositorExtensionTemplate<InputPanelV1>, public QtWaylandServer::zwp_input_panel_v1
{
    Q_OBJECT

public:
    explicit InputPanelV1(QWaylandCompositor *compositor)
        : QWaylandCompositorExtensionTemplate<InputPanelV1>(compositor)
    {
    }

    void initialize() override
    {
        QWaylandCompositorExtensionTemplate::initialize();
        auto *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
        if (!compositor) {
            qWarning() << "No compositor available when initializing input panel";
            return;
        }
        init(compositor->display(), interfaceVersion());
    }

    int overlayPanelCount() const
    {
        return m_overlayPanelCount;
    }

    int toplevelPanelCount() const
    {
        return m_toplevelPanelCount;
    }

    wl_resource *lastSurfaceResource() const
    {
        return m_lastSurfaceResource;
    }

Q_SIGNALS:
    void inputPanelSurfaceCreated();
    void overlayPanelRequested();
    void toplevelPanelRequested();

protected:
    void zwp_input_panel_v1_get_input_panel_surface(Resource *resource, uint32_t id, wl_resource *surface) override
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

private:
    int m_overlayPanelCount = 0;
    int m_toplevelPanelCount = 0;
    wl_resource *m_lastSurfaceResource = nullptr;
};

class InputMethodV1 : public QWaylandCompositorExtensionTemplate<InputMethodV1>, public QtWaylandServer::zwp_input_method_v1
{
    Q_OBJECT
public:
    explicit InputMethodV1(QWaylandCompositor *compositor)
        : QWaylandCompositorExtensionTemplate<InputMethodV1>(compositor)
    {
    }

    void setInputPanel(InputPanelV1 *inputPanel)
    {
        m_inputPanel = inputPanel;
    }

    void initialize() override
    {
        QWaylandCompositorExtensionTemplate::initialize();
        auto *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
        if (!compositor) {
            qWarning() << "No compositor available when initializing input method";
            return;
        }
        init(compositor->display(), interfaceVersion());
    }

    void sendActivate()
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

    void sendDeactivate()
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

    InputMethodContext *context() const
    {
        return m_context.get();
    }

Q_SIGNALS:
    void activated();

protected:
    void zwp_input_method_v1_bind_resource(Resource *resource) override
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

private:
    const bool m_autoActivate = true;
    const int m_activateDelayMs = 200;

    bool m_activatePending = false;
    std::unique_ptr<InputMethodContext> m_context;
    InputPanelV1 *m_inputPanel = nullptr;
};

class MockInputMethodCompositorTest : public QObject
{
    Q_OBJECT

public:
    MockInputMethodCompositorTest() = default;

private Q_SLOTS:
    void initTestCase()
    {
        // create a temporary folder for test configs
        if (!m_home.isValid() || !qputenv("HOME", qPrintable(m_home.path()))) {
            qFatal("Couldn't create temporary home folder for the test");
            return;
        }

        {
            KConfig cfg(QStringLiteral("plasmakeyboardrc"));
            KConfigGroup grp(&cfg, QStringLiteral("General"));
            grp.writeEntry(QStringLiteral("enabledLocales"), QStringLiteral("it_IT"));
        }

        m_compositor = std::make_unique<QWaylandCompositor>();
        m_socketPath = m_runtimeDir.path() + QLatin1String("/plasma-keyboard-mock-") + QString::number(QCoreApplication::applicationPid());
        m_compositor->setSocketName(m_socketPath.toUtf8());
        m_compositor->setUseHardwareIntegrationExtension(false);
        m_compositor->create();
        QTRY_VERIFY_WITH_TIMEOUT(m_compositor->isCreated(), 2000);

        m_outputWindow = std::make_unique<QWindow>();
        m_outputWindow->setGeometry(0, 0, 1280, 720);
        m_outputWindow->setTitle(u"Mock Compositor Output"_s);
        m_outputWindow->setVisible(false);

        m_output = std::make_unique<QWaylandOutput>(m_compositor.get(), m_outputWindow.get());
        m_output->setManufacturer(u"Mock"_s);
        m_output->setModel(u"InputMethod"_s);
        m_output->addMode(QWaylandOutputMode(QSize(1280, 720), 60000), true);
        m_output->setCurrentMode(m_output->modes().first());
        m_compositor->setDefaultOutput(m_output.get());

        m_xdgShell = std::make_unique<QWaylandXdgShell>(m_compositor.get());
        m_xdgShell->initialize();

        m_inputMethod = std::make_unique<InputMethodV1>(m_compositor.get());
        m_inputMethod->initialize();

        m_inputPanel = std::make_unique<InputPanelV1>(m_compositor.get());
        m_inputPanel->initialize();
        m_inputMethod->setInputPanel(m_inputPanel.get());

        m_child = std::make_unique<QProcess>();
        connect(m_child.get(), &QProcess::readyReadStandardError, this, [this] {
            QTextStream(stderr) << m_child->readAllStandardError();
        });
        connect(m_child.get(), &QProcess::readyReadStandardOutput, this, [this] {
            QTextStream(stdout) << m_child->readAllStandardOutput();
        });
        connect(m_child.get(), &QProcess::finished, this, [this] {
            qWarning() << "child state:" << m_child->state() << "error:" << m_child->error() << m_child->errorString() << "exitStatus:" << m_child->exitStatus()
                       << "exitCode:" << m_child->exitCode();
        });
        auto env = QProcessEnvironment::systemEnvironment();
        env.insert(u"WAYLAND_DISPLAY"_s, m_socketPath);
        env.insert(u"QT_QUICK_BACKEND"_s, u"software"_s); // Without this plasma-keyboard explodes on alpine for some reason
        m_child->setProcessEnvironment(env);

#if PLASMA_KEYBOARD_UNDER_GDB
        m_child->setProgram(QStringLiteral("gdb"));
        m_child->setArguments({QStringLiteral("-batch"),
                               QStringLiteral("-ex"),
                               QStringLiteral("set pagination off"),
                               QStringLiteral("-ex"),
                               QStringLiteral("run"),
                               QStringLiteral("-ex"),
                               QStringLiteral("bt"),
                               QStringLiteral("-ex"),
                               QStringLiteral("quit"),
                               QStringLiteral("--args"),
                               QStringLiteral(PLASMA_KEYBOARD_BINARY_PATH)});
#else
        m_child->setProgram(QStringLiteral(PLASMA_KEYBOARD_BINARY_PATH));
#endif
        m_child->start();
        QVERIFY2(m_child->waitForStarted(), qPrintable(m_child->errorString()));

        qInfo().noquote().nospace() << "Compositor running on WAYLAND_DISPLAY=" << m_socketPath << " " << PLASMA_KEYBOARD_BINARY_PATH;
        {
            QSignalSpy spy(m_inputMethod.get(), &InputMethodV1::activated);
            QVERIFY(spy.count() || spy.wait());
        }

        QVERIFY(m_inputMethod->context());
        if (!m_inputMethod->context()->keyboard()) {
            QSignalSpy grabSpy(m_inputMethod->context(), &InputMethodContext::keyboardGrabbed);
            QVERIFY(grabSpy.wait());
        }

        auto *keyboard = m_inputMethod->context()->keyboard();
        QVERIFY(keyboard);
        if (!keyboard->keymapped()) {
            QSignalSpy grabSpy(keyboard, &InputMethodKeyboard::keymapDone);
            QVERIFY(grabSpy.wait());
        }
    }

    void sendKey(int key, int interval)
    {
        auto keyboard = m_inputMethod->context()->keyboard();
        Q_ASSERT(keyboard);
        keyboard->sendKey(key, WL_KEYBOARD_KEY_STATE_PRESSED);
        wl_display_flush_clients(m_compositor->display());
        QTest::qWait(interval);
        keyboard->sendKey(key, WL_KEYBOARD_KEY_STATE_RELEASED);
        wl_display_flush_clients(m_compositor->display());
    }

    void testLongPressShowsOverlayPanel()
    {
        QSignalSpy overlaySpy(m_inputPanel.get(), &InputPanelV1::overlayPanelRequested);

        sendKey(KEY_A, 1200);
        QVERIFY(overlaySpy.count() || overlaySpy.wait());

        QSignalSpy commitStringSpy(m_inputMethod->context(), &InputMethodContext::commitStringChanged);
        sendKey(KEY_1, 10);
        QVERIFY(commitStringSpy.count() || commitStringSpy.wait());
        QCOMPARE(commitStringSpy.count(), 1);
        QCOMPARE(commitStringSpy.first().first().toString(), QStringLiteral("à"));
    }

    void cleanupTestCase()
    {
        if (m_child) {
            if (m_child->state() != QProcess::NotRunning) {
                m_child->terminate();
                m_child->waitForFinished(2000);
            }
            m_child.reset();
        }
    }

private:
    QTemporaryDir m_runtimeDir;
    QTemporaryDir m_home;
    QString m_socketPath;

    std::unique_ptr<QWaylandCompositor> m_compositor;
    std::unique_ptr<QWindow> m_outputWindow;
    std::unique_ptr<QWaylandOutput> m_output;
    std::unique_ptr<QWaylandXdgShell> m_xdgShell;
    std::unique_ptr<InputMethodV1> m_inputMethod;
    std::unique_ptr<InputPanelV1> m_inputPanel;
    std::unique_ptr<QProcess> m_child;
};

QTEST_MAIN(MockInputMethodCompositorTest)

#include "mockinputmethodcompositor.moc"
