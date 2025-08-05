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
    Q_PROPERTY(QRect interactiveRegion READ interactiveRegion WRITE setInteractiveRegion NOTIFY interactiveRegionChanged)

public:
    InputPanelWindow(QWindow *parent = nullptr);

    // The interactive part of the keyboard that is reserved on the screen for the input panel.
    // The space outside of it will be overlaid will have input passed to underlying windows by the compositor.
    // This is required for some visual items (ex. popovers) to show outside of the input panel.
    QRect interactiveRegion() const;
    void setInteractiveRegion(QRect interactiveRegion);

Q_SIGNALS:
    void interactiveRegionChanged();

private:
    QRect m_interactiveRegion;
};
