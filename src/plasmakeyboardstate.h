/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>

class PlasmaKeyboardState : public QObject
{
    Q_OBJECT

    // Updated by KWinFakeInput
    Q_PROPERTY(bool modifierMetaPressed READ modifierMetaPressed NOTIFY modifierMetaPressedChanged)
    Q_PROPERTY(bool modifierCapsLockPressed READ modifierCapsLockPressed NOTIFY modifierCapsLockPressedChanged)
    Q_PROPERTY(bool modifierControlPressed READ modifierControlPressed NOTIFY modifierControlPressedChanged)
    Q_PROPERTY(bool modifierAltPressed READ modifierAltPressed NOTIFY modifierAltPressedChanged)
    Q_PROPERTY(bool modifierShiftPressed READ modifierShiftPressed NOTIFY modifierShiftPressedChanged)

public:
    static PlasmaKeyboardState *self();

    bool modifierMetaPressed() const;
    void setModifierMetaPressed(bool pressed);

    bool modifierCapsLockPressed() const;
    void setModifierCapsLockPressed(bool pressed);

    bool modifierControlPressed() const;
    void setModifierControlPressed(bool pressed);

    bool modifierAltPressed() const;
    void setModifierAltPressed(bool pressed);

    bool modifierShiftPressed() const;
    void setModifierShiftPressed(bool pressed);

Q_SIGNALS:
    void modifierMetaPressedChanged();
    void modifierCapsLockPressedChanged();
    void modifierControlPressedChanged();
    void modifierAltPressedChanged();
    void modifierShiftPressedChanged();

private:
    bool m_modifierMetaPressed = false;
    bool m_modifierCapsLockPressed = false;
    bool m_modifierControlPressed = false;
    bool m_modifierAltPressed = false;
    bool m_modifierShiftPressed = false;
};
