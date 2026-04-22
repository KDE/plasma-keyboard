/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QPointer>
#include <QWindow>
#include <qqmlintegration.h>

#include "inputplugin.h"

class OverlayController;
class KWinFakeInput;
class VirtualKeyboardContext;
class VkbdWaylandInputBackend;

class InputMethodConnection : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(InputMethodConnection)

    Q_PROPERTY(QWindow *window READ window WRITE setWindow NOTIFY windowChanged)
    Q_PROPERTY(bool keyboardNavigationActive READ keyboardNavigationActive WRITE setKeyboardNavigationActive NOTIFY keyboardNavigationActiveChanged)
    Q_PROPERTY(VirtualKeyboardContext *virtualKeyboardContext READ virtualKeyboardContext CONSTANT)

    /**
     * Controller for overlay popups (diacritics, emoji, text expansion).
     *
     * Exposed to QML for connecting overlay windows.
     */
    Q_PROPERTY(OverlayController *overlayController READ overlayController CONSTANT)

public:
    explicit InputMethodConnection(QObject *parent = nullptr);

    QWindow *window() const;
    void setWindow(QWindow *window);
    bool keyboardNavigationActive() const;
    void setKeyboardNavigationActive(bool active);
    Q_INVOKABLE void hide();

    VirtualKeyboardContext *virtualKeyboardContext() const;

    /**
     * Get the overlay controller.
     */
    OverlayController *overlayController() const;

Q_SIGNALS:
    void windowChanged();
    void keyboardNavigationActiveChanged();
    void keyNavigationPressed(int key);
    void keyNavigationReleased(int key);

private:
    void updateWindowVisible();

    InputPlugin m_input;
    VkbdWaylandInputBackend *m_vkbdInputBackend = nullptr;
    VirtualKeyboardContext *m_virtualKeyboardContext = nullptr;
    KWinFakeInput *m_fakeInput = nullptr;
    OverlayController *m_overlayController = nullptr;
    QPointer<QWindow> m_window;
    bool m_hidden = false;
    bool m_keyboardNavigationActive = false;
};
