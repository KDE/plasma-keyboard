/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <KAboutData>
#include <KLocalizedString>

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQuickItem>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QWindow>
#include <QTextFormat>
#include <QVirtualKeyboardAbstractInputMethod>
#include <QVirtualKeyboardInputContext>
#include <QVirtualKeyboardInputEngine>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <qpa/qwindowsysteminterface.h>

#include <set>

#include "inputmethod_p.h"
#include "inputplugin.h"
#include "qwaylandinputpanelshellintegration_p.h"
#include "qwaylandinputpanelsurface_p.h"

Q_GLOBAL_STATIC(InputMethod, s_im)

static const std::set<int> IGNORED_KEYS = {
    Qt::Key_Context1 // Triggered by "special keys" button
};

class InputThing : public QQuickItem
{
    Q_OBJECT
    // QML_ELEMENT
    Q_PROPERTY(QVirtualKeyboardInputEngine *engine WRITE setEngine)

public:
    InputThing()
        : m_input(&(*s_im))
    {
        connect(&m_input, &InputPlugin::contextChanged, this, [this] {
            if (m_input.hasContext()) {
                QGuiApplication::inputMethod()->update(Qt::ImQueryAll);
                QGuiApplication::inputMethod()->show();
            } else {
                QGuiApplication::inputMethod()->setVisible(false);
            }
        });
        connect(&m_input, &InputPlugin::surroundingTextChanged, this, [this] {
            QGuiApplication::inputMethod()->update(Qt::ImSurroundingText);
        });
        connect(&m_input, &InputPlugin::receivedCommit, this, [] {
            QGuiApplication::inputMethod()->setVisible(true);
        });
        connect(QGuiApplication::inputMethod(), &QInputMethod::visibleChanged, this, [this] {
            window()->setVisible(QGuiApplication::inputMethod()->isVisible());
        });
        QGuiApplication::inputMethod()->update(Qt::ImQueryAll);
    }

    void setEngine(QVirtualKeyboardInputEngine *engine) {
        // TODO: hook into engine events if necessary?
    }

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override
    {
        if (!m_input.hasContext())
            return {};

        switch (query) {
        case Qt::ImEnabled:
            return true;
        case Qt::ImSurroundingText:
            return m_input.surroundingText();
        case Qt::ImHints: {
            const auto imHints = m_input.contentHint();
            Qt::InputMethodHints qtHints;

            // if (imHints & InputPlugin::content_hint_default) { }
            if (imHints & InputPlugin::content_hint_password) {
                qtHints |= Qt::ImhSensitiveData;
            }
            if ((imHints & InputPlugin::content_hint_auto_completion) == 0) {
                qtHints |= Qt::ImhNoPredictiveText;
            }
            if ((imHints & InputPlugin::content_hint_auto_correction) == 0 || (imHints & InputPlugin::content_hint_auto_capitalization) == 0) {
                qtHints |= Qt::ImhNoAutoUppercase;

            }
            // if (imHints & InputPlugin::content_hint_titlecase) { }
            if (imHints & InputPlugin::content_hint_lowercase) {
                qtHints |= Qt::ImhPreferLowercase;
            }
            if (imHints & InputPlugin::content_hint_uppercase) {
                qtHints |= Qt::ImhPreferUppercase;
            }
            if (imHints & InputPlugin::content_hint_hidden_text) {
                qtHints |= Qt::ImhSensitiveData;
            }
            if (imHints & InputPlugin::content_hint_sensitive_data) {
                qtHints |= Qt::ImhSensitiveData;
            }
            if (imHints & InputPlugin::content_hint_latin) {
                qtHints |= Qt::ImhPreferLatin;
            }
            if (imHints & InputPlugin::content_hint_multiline) {
                qtHints |= Qt::ImhMultiLine;
            }
            const auto imPurpose = m_input.contentPurpose();
            switch (imPurpose) {
                case InputPlugin::content_purpose_normal:
                case InputPlugin::content_purpose_alpha:
                case InputPlugin::content_purpose_name:
                    break;
                case InputPlugin::content_purpose_digits:
                    qtHints |= Qt::ImhDigitsOnly;
                    break;
                case InputPlugin::content_purpose_number:
                    qtHints |= Qt::ImhPreferNumbers;
                    break;
                case InputPlugin::content_purpose_phone:
                    qtHints |= Qt::ImhDialableCharactersOnly;
                    break;
                case InputPlugin::content_purpose_url:
                    qtHints |= Qt::ImhUrlCharactersOnly;
                    break;
                case InputPlugin::content_purpose_email:
                    qtHints |= Qt::ImhEmailCharactersOnly;
                    break;
                case InputPlugin::content_purpose_password:
                    qtHints |= Qt::ImhSensitiveData;
                    break;
                case InputPlugin::content_purpose_date:
                    qtHints |= Qt::ImhDate;
                    break;
                case InputPlugin::content_purpose_time:
                    qtHints |= Qt::ImhTime;
                    break;
                case InputPlugin::content_purpose_datetime:
                    qtHints |= Qt::ImhDate;
                    qtHints |= Qt::ImhTime;
                    break;
                case InputPlugin::content_purpose_terminal:
                    qtHints |= Qt::ImhPreferLatin;
                    break;
            }
            return QVariant::fromValue<int>(qtHints);
        } break;
        case Qt::ImCurrentSelection:
            return m_input.surroundingText().mid(m_input.cursorPos(), m_input.anchorPos());
        case Qt::ImAnchorPosition:
        case Qt::ImAnchorRectangle:
        case Qt::ImCursorPosition:
        case Qt::ImCursorRectangle:
        case Qt::ImInputItemClipRectangle:
            // We don't do that
            break;
        default:
            qWarning() << "Unhandled query" << query;
            break;
        }
        return {};
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        if (IGNORED_KEYS.find(event->key()) != IGNORED_KEYS.end()) {
            return;
        }

        QList<xkb_keysym_t> keys = QXkbCommon::toKeysym(event);
        for (auto key : keys) {
            // Simulate key press only if it's not textual
            if (event->text().isEmpty() || key == XKB_KEY_Return) { // (return is technically "\n")
                m_input.keysym(QDateTime::currentMSecsSinceEpoch(), key, InputPlugin::Pressed, 0);
            }
        }
    }

    void keyReleaseEvent(QKeyEvent *event) override
    {
        if (IGNORED_KEYS.find(event->key()) != IGNORED_KEYS.end()) {
            return;
        }

        QList<xkb_keysym_t> keys = QXkbCommon::toKeysym(event);
        for (auto key : keys) {
            if (event->text().isEmpty() || key == XKB_KEY_Return) { // (return is technically "\n")
                // Simulate the keyboard press for non textual keys
                m_input.keysym(QDateTime::currentMSecsSinceEpoch(), key, InputPlugin::Released, 0);
            } else {
                // If we have text coming as a key event, use it to commit the string
                m_input.commit(event->text());
            }
        }
    }

    void inputMethodEvent(QInputMethodEvent *event) override
    {
        // Delete characters that are supposed to be replaced
        if (event->replacementLength() > 0) {
            m_input.deleteSurroundingText(event->replacementStart(), event->replacementLength());
        }

        for (auto x : event->attributes()) {
            if (x.type == QInputMethodEvent::TextFormat) {
                m_input.setPreEditStyle(x.start, x.length, x.value.value<QTextFormat>().type());
            }
        }

        // Send cursor position (must be before predit string)
        m_input.setPreEditCursor(event->preeditString().size());

        // Send currently being edited string
        m_input.setPreEditString(event->preeditString());

        // Commit string if we have a finished string
        if (const auto commit = event->commitString(); !commit.isEmpty()) {
            m_input.commit(commit);
        }
    }

private:
    InputPlugin m_input;
};

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

    qmlRegisterType<InputThing>("org.kde.plasma.keyboard", 1, 0, "InputThing");

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

#include "main.moc"
