/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <qqmlintegration.h>

/**
 * @file
 *
 * QML-visible enum for describing how an input panel surface should be treated by the compositor.
 */

/**
 * Exposes input panel role values to both C++ and QML.
 */
class InputPanelRole : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("InputPanelRole is an enum container and cannot be instantiated.")

public:
    /**
     * Declares how the window should be exposed to the compositor.
     *
     * The role influences compositor behavior such as placement, focus interactions,
     * and stacking relative to other surfaces.
     */
    enum Role {
        /** Main on-screen keyboard window. */
        Keyboard = 0,
        /** Overlay panel associated with and in proximity to a text input field, such as a diacritics/emoji popup. */
        OverlayPanel = 1,
    };
    Q_ENUM(Role)
};
