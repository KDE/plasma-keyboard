/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "inputpanelwindow.h"

InputPanelWindow::InputPanelWindow(QWindow *parent)
    : QQuickWindow{parent}
{}

qreal InputPanelWindow::interactiveHeight() const
{
    return m_interactiveHeight;
}

void InputPanelWindow::setInteractiveHeight(qreal interactiveHeight)
{
    if (interactiveHeight == m_interactiveHeight) {
        return;
    }
    m_interactiveHeight = interactiveHeight;
    Q_EMIT interactiveHeightChanged();

    // Set only a part of the window to be interactive
    setMask(QRegion(0, height() - interactiveHeight, width(), interactiveHeight));
}
