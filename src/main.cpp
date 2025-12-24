/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "logging.h"

#include <KAboutData>
#include <KConfigWatcher>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

#include "plasmakeyboardsettings.h"
#include <QCommandLineParser>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QWindow>

#include "inputpanelintegration.h"

int main(int argc, char **argv)
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_WAYLAND_INPUT_PANEL_TOPLEVEL", QByteArray("1"));

    // PLASMA_KEYBOARD_USE_QT_LAYOUTS - whether to use Qt's builtin keyboard layouts rather than our own.
    bool useQtLayouts = QByteArrayList{"1", "true"}.contains(qgetenv("PLASMA_KEYBOARD_USE_QT_LAYOUTS").toLower());

    if (!useQtLayouts) {
        // Set QT_VIRTUALKEYBOARD_LAYOUT_PATH to our own keyboard layouts provided in this repository.

        // Loop over all "/usr/share" paths and check if layouts folder exists
        const QStringList locations = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
        for (const QString &basePath : locations) {
            QString layoutsDir = basePath + QStringLiteral("/plasma/keyboard/layouts");

            // Check if path exists
            if (QDir(layoutsDir).exists()) {
                // Set path for Qt to search for layouts
                qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", layoutsDir.toUtf8());
                break;
            }
        }
    }

    QGuiApplication application(argc, argv);

    KLocalizedString::setApplicationDomain("plasma-keyboard");

    KAboutData aboutData(QStringLiteral("plasma-keyboard"),
                         i18n("Plasma Keyboard"),
                         QStringLiteral("0.1.0"),
                         i18n("An on-screen keyboard for Plasma"),
                         KAboutLicense::GPL,
                         i18n("Copyright 2024, Aleix Pol Gonzalez"));

    aboutData.addAuthor(i18n("Aleix Pol Gonzalez"), i18n("Author"), QStringLiteral("aleixpol@kde.org"));
    aboutData.setOrganizationDomain("kde.org");
    aboutData.setDesktopFileName(QStringLiteral("org.kde.plasma.keyboard"));
    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("input-keyboard-virtual")));
    aboutData.setProgramLogo(application.windowIcon());

    KAboutData::setApplicationData(aboutData);

    {
        QCommandLineParser parser;
        aboutData.setupCommandLine(&parser);
        parser.process(application);
        aboutData.processCommandLine(&parser);
    }

    // Listen to config updates from kcm, and reparse
    auto watcher = KConfigWatcher::create(PlasmaKeyboardSettings::self()->sharedConfig());
    // clang-format off
    QObject::connect(watcher.get(),
        &KConfigWatcher::configChanged,
        &application,
        [](const KConfigGroup &, const QByteArrayList &) {
            PlasmaKeyboardSettings::self()->sharedConfig()->reparseConfiguration();
            PlasmaKeyboardSettings::self()->load();
        });
    // clang-format on

    QQmlApplicationEngine view;
    KLocalization::setupLocalizedContext(&view);

    QObject::connect(&view, &QQmlApplicationEngine::objectCreated, &application, [](QObject *object, const QUrl &) {
        auto window = qobject_cast<QWindow *>(object);
        const bool ok = initInputPanelIntegration(window, InputPanelRole::Keyboard);

        if (!ok) {
            QTextStream(stderr) << "Cannot run plasma-keyboard standalone. You can enable it in Plasma’s System Settings app, on the “Virtual Keyboard” page.";
            exit(1);
        }

        window->requestActivate();
        window->setVisible(true);
    });
    view.load(QUrl(QStringLiteral("qrc:/qt/qml/org/kde/plasma/keyboard/main.qml")));

    qCDebug(PlasmaKeyboard) << "Starting Plasma Keyboard application";

    return application.exec();
}
