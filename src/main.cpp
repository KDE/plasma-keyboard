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

#include "inputmethod_p.h"
#include "inputplugin.h"
#include "qwaylandinputpanelshellintegration_p.h"
#include "qwaylandinputpanelsurface_p.h"

Q_GLOBAL_STATIC(InputMethod, s_im)

class InputThing : public QQuickItem
{
    Q_OBJECT
    // QML_ELEMENT
public:
    InputThing()
        : m_input(&(*s_im))
    {
        connect(&m_input, &InputPlugin::contextChanged, this, [this] {
            qDebug() << "input context changed";
            if (m_input.hasContext()) {
                QGuiApplication::inputMethod()->update(Qt::ImQueryAll);
                QGuiApplication::inputMethod()->show();
            } else {
                QGuiApplication::inputMethod()->setVisible(false);
            }
        });
        connect(&m_input, &InputPlugin::surroundingTextChanged, this, [this] {
            qDebug() << "surroundingText changed" << m_input.surroundingText();
            QGuiApplication::inputMethod()->update(Qt::ImSurroundingText);
        });
        connect(&m_input, &InputPlugin::receivedCommit, this, [this] {
            QGuiApplication::inputMethod()->setVisible(true);
        });
        connect(QGuiApplication::inputMethod(), &QInputMethod::visibleChanged, this, [this] {
            qDebug() << "ffffffff" << QGuiApplication::inputMethod()->isVisible();
            window()->setVisible(QGuiApplication::inputMethod()->isVisible());
        });
        QGuiApplication::inputMethod()->update(Qt::ImQueryAll);
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
            qDebug() << "hintssssssss" << qtHints;
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
            qDebug() << "query" << query;
            break;
        }
        return {};
    }
    void inputMethodEvent(QInputMethodEvent *event) override
    {
        qDebug() << "event" << event << event->commitString();
        m_input.setPreEditStyle(event->replacementStart(), event->replacementLength(), 0);
        m_input.setPreEditString(event->preeditString());
        for (auto x : event->attributes()) {
            qDebug() << "mmmmmmmmmmm" << x.type << x.start << x.length << x.value;
        }
        // m_input.setPreEditCursor(event->curso);
        if (const auto commit = event->commitString(); !commit.isEmpty()) {
            m_input.commit(commit);
        }
    }

private:
    InputPlugin m_input;
};

class WaylandInputMethod : public QVirtualKeyboardAbstractInputMethod
{
    Q_OBJECT
public:
    WaylandInputMethod(QObject *parent)
        : QVirtualKeyboardAbstractInputMethod(parent)
    {
    }

    QList<QVirtualKeyboardInputEngine::InputMode> inputModes(const QString &locale) override
    {
        qDebug() << "xxxxxxxx woooo" << locale;
        return {};
    }
    bool setInputMode(const QString &locale, QVirtualKeyboardInputEngine::InputMode inputMode) override
    {
        qDebug() << "xxxxxxxx wippy" << locale << inputMode;
        return true;
    }
    bool setTextCase(QVirtualKeyboardInputEngine::TextCase textCase) override
    {
        qDebug() << "xxxxxxxx text case!" << textCase;
        return true;
    }

    bool keyEvent(Qt::Key key, const QString &text, Qt::KeyboardModifiers modifiers) override
    {
        qDebug() << "xxxxxxxx key event" << key << text << modifiers;
        // const Qt::KeyboardModifiers mods = (key == Qt::Key_Return) ? Qt::NoModifier : modifiers;
        // inputContext()->sendKeyClick(key, text, mods);
        // QKeyEvent keyEvent();
        // m_input.gr();
        return true;
    }
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

    KLocalizedString::setApplicationDomain("qvk");

    KAboutData aboutData(QStringLiteral("qvk"),
                         i18n("qvk"),
                         QStringLiteral("1.0"),
                         i18n("A Simple Application written with KDE Frameworks"),
                         KAboutLicense::GPL,
                         i18n("Copyright 2024, Aleix Pol Gonzalez"));

    aboutData.addAuthor(i18n("Aleix Pol Gonzalez"), i18n("Author"), QStringLiteral("aleixpol@kde.org"));
    aboutData.setOrganizationDomain("example.org");
    aboutData.setDesktopFileName(QStringLiteral("org.example.qvk"));

    KAboutData::setApplicationData(aboutData);

    {
        QCommandLineParser parser;
        aboutData.setupCommandLine(&parser);
        parser.process(application);
        aboutData.processCommandLine(&parser);
    }

    qmlRegisterType<InputThing>("org.kde.qvk", 1, 0, "InputThing");

    QQmlApplicationEngine view;
    QObject::connect(&view, &QQmlApplicationEngine::objectCreated, &application, [] (QObject *object) {
        auto engine = static_cast<QVirtualKeyboardInputEngine *>(object->property("engine").value<QObject *>());
        engine->setInputMethod(new WaylandInputMethod(engine));

        auto window = qobject_cast<QWindow*>(object);
        if (!initPanelIntegration(window)) {
            qDebug() << "aaaaaaaaaaaa";
            QCoreApplication::instance()->exit(1);
            Q_UNREACHABLE();
        }
        QObject::connect(engine, &QVirtualKeyboardInputEngine::virtualKeyClicked, object, [] (Qt::Key key, const QString &text, Qt::KeyboardModifiers modifiers, bool isAutoRepeat) {
            qDebug() << "fffffffff" << key << text << modifiers << isAutoRepeat;
        });
    });
    view.load(QUrl(QStringLiteral("qrc:/qt/qml/org/kde/qvk/main.qml")));

    return application.exec();
}

#include "main.moc"
