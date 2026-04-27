// SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "plasmakeyboardtestcase.h"

#include "plasmakeyboard_test_config.h"

#include <QCoreApplication>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTextStream>
#include <QtTest/QTest>
#include <QtWaylandCompositor/QWaylandOutputMode>

#include <linux/input-event-codes.h>
#include <wayland-server-core.h>

using namespace Qt::StringLiterals;

// Set to 1 to run plasma-keyboard under GDB and print a backtrace on crash.
#define PLASMA_KEYBOARD_UNDER_GDB 0

void PlasmaKeyboardTestCase::initTestCase()
{
    if (!m_home.isValid() || !qputenv("HOME", qPrintable(m_home.path()))) {
        qFatal("Couldn't create temporary home folder for the test");
        return;
    }
    QStandardPaths::setTestModeEnabled(true);

    setupConfig();

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
        QSignalSpy keymapSpy(keyboard, &InputMethodKeyboard::keymapDone);
        QVERIFY(keymapSpy.wait());
    }
}

void PlasmaKeyboardTestCase::cleanupTestCase()
{
    if (m_child) {
        if (m_child->state() != QProcess::NotRunning) {
            m_child->terminate();
            m_child->waitForFinished(2000);
        }
        m_child.reset();
    }
}

void PlasmaKeyboardTestCase::sendKey(int key, int interval)
{
    auto *keyboard = m_inputMethod->context()->keyboard();
    Q_ASSERT(keyboard);
    keyboard->sendKey(key, WL_KEYBOARD_KEY_STATE_PRESSED);
    wl_display_flush_clients(m_compositor->display());
    QTest::qWait(interval);
    keyboard->sendKey(key, WL_KEYBOARD_KEY_STATE_RELEASED);
    wl_display_flush_clients(m_compositor->display());
}

#include "plasmakeyboardtestcase.moc"
