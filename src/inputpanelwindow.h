/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QQuickWindow>

class InputPanelWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(qreal interactiveHeight READ interactiveHeight WRITE setInteractiveHeight NOTIFY interactiveHeightChanged)

public:
    InputPanelWindow(QWindow *parent = nullptr);

    // The height of the interactive part of the keyboard that is reserved on the screen for the input panel, from the bottom.
    // The space above it will be overlaid over the application window by the compositor.
    qreal interactiveHeight() const;
    void setInteractiveHeight(qreal interactiveHeight);

Q_SIGNALS:
    void interactiveHeightChanged();

private:
    qreal m_interactiveHeight;
};
