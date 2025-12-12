/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QQuickItem>
#include <QQuickWindow>
#include <QTimer>
#include <QVirtualKeyboardInputEngine>
#include <qqmlintegration.h>

#include "inputplugin.h"

class InputListenerItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVirtualKeyboardInputEngine *engine WRITE setEngine)
    Q_PROPERTY(bool keyboardNavigationActive MEMBER m_keyboardNavigationActive)

public:
    InputListenerItem();

    void setEngine(QVirtualKeyboardInputEngine *engine);

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void inputMethodEvent(QInputMethodEvent *event) override;

Q_SIGNALS:
    void keyNavigationPressed(int key);
    void keyNavigationReleased(int key);
    void diacriticsPopupRequested(const QString &baseCharacter);
    void diacriticsPopupCancelled();

public Q_SLOTS:
    Q_INVOKABLE void commitDiacritic(const QString &text);

private Q_SLOTS:
    void handleHoldTimeout();

private:
    bool shouldHandleDiacritics(const QKeyEvent *event) const;
    void resetPendingDiacriticsState();

    InputPlugin m_input;
    bool m_keyboardNavigationActive = false;
    QTimer m_holdTimer;
    QString m_pendingText;
    int m_pendingKey = 0;
    bool m_popupShown = false;
    bool m_popupDismissed = false;
    bool m_selectionMade = false;
};
