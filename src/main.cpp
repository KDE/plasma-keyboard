/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <KAboutData>
#include <KLocalizedString>
#include <KConfigWatcher>

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QWindow>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <qpa/qwindowsysteminterface.h>

#include "inputlisteneritem.h"
#include "inputpanelwindow.h"
#include "qwaylandinputpanelshellintegration_p.h"
#include "qwaylandinputpanelsurface_p.h"
#include "plasmakeyboardsettings.h"
#include "vibration.h"

static bool initPanelIntegration(QWindow *window)
{
    window->create();
    auto waylandWindow = dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle());
    if (!waylandWindow) {
        qWarning() << window << "is not a wayland window. Not creating panel";
        return false;
    }
    static QWaylandInputPanelShellIntegration *shellIntegration = nullptr;
    if (!shellIntegration) {
        shellIntegration = new QWaylandInputPanelShellIntegration();
        if (!shellIntegration->initialize(waylandWindow->display())) {
            delete shellIntegration;
            shellIntegration = nullptr;
            qWarning() << "Failed to initialize input panel-shell integration, possibly because compositor does not support the layer-shell protocol";
            return false;
        }
    }
    waylandWindow->setShellIntegration(shellIntegration);
    window->requestActivate();
    window->setVisible(true);
    QWindowSystemInterface::handleFocusWindowChanged(window);
    return true;
}

int main(int argc, char **argv)
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_WAYLAND_INPUT_PANEL_TOPLEVEL", QByteArray("1"));
    QGuiApplication application(argc, argv);

    KLocalizedString::setApplicationDomain("plasma-keyboard");

    KAboutData aboutData(QStringLiteral("plasma-keyboard"),
                         i18n("plasma-keyboard"),
                         QStringLiteral("1.0"),
                         i18n("A Simple Application written with KDE Frameworks"),
                         KAboutLicense::GPL,
                         i18n("Copyright 2024, Aleix Pol Gonzalez"));

    aboutData.addAuthor(i18n("Aleix Pol Gonzalez"), i18n("Author"), QStringLiteral("aleixpol@kde.org"));
    aboutData.setOrganizationDomain("kde.org");
    aboutData.setDesktopFileName(QStringLiteral("org.kde.plasma.keyboard"));

    KAboutData::setApplicationData(aboutData);

    {
        QCommandLineParser parser;
        aboutData.setupCommandLine(&parser);
        parser.process(application);
        aboutData.processCommandLine(&parser);
    }

    // Listen to config updates from kcm, and reparse
    auto watcher = KConfigWatcher::create(PlasmaKeyboardSettings::self()->sharedConfig());
    QObject::connect(watcher.get(),
        static_cast<void (KConfigWatcher::*)(const KConfigGroup &, const QByteArrayList &)>(&KConfigWatcher::configChanged),
        &application,
        [](const KConfigGroup &, const QByteArrayList &) {
            PlasmaKeyboardSettings::self()->sharedConfig()->reparseConfiguration();
            PlasmaKeyboardSettings::self()->load();
        });

    Vibration vibration;

    qmlRegisterType<InputListenerItem>("org.kde.plasma.keyboard", 1, 0, "InputListenerItem");
    qmlRegisterType<InputPanelWindow>("org.kde.plasma.keyboard", 1, 0, "InputPanelWindow");
    qmlRegisterSingletonInstance<PlasmaKeyboardSettings>("org.kde.plasma.keyboard", 1, 0,
        "PlasmaKeyboardSettings", PlasmaKeyboardSettings::self());
    qmlRegisterSingletonInstance<Vibration>("org.kde.plasma.keyboard", 1, 0,
        "Vibration", &vibration);

    QQmlApplicationEngine view;
    QObject::connect(&view, &QQmlApplicationEngine::objectCreated, &application, [] (QObject *object) {
        auto window = qobject_cast<QWindow*>(object);
        if (!initPanelIntegration(window)) {
            QCoreApplication::instance()->exit(1);
            Q_UNREACHABLE();
        }
    });
    view.load(QUrl(QStringLiteral("qrc:/qt/qml/org/kde/plasma/keyboard/main.qml")));

    return application.exec();
}

