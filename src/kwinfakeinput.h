/*
    SPDX-FileCopyrightText: 2022 Bart Ribbers <bribbers@disroot.org>
    SPDX-FileCopyrightText: 2022 Aleix Pol Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QSet>

class FakeInput;

class KWinFakeInput : public QObject
{
    Q_OBJECT

public:
    bool init();

    // Whether a Qt key should be sent over fake_input instead of input_method_v1
    bool shouldUseFakeInput(int key) const;

    // Whether a Qt key is a modifier key sent over fake_input
    bool isModifier(int key) const;

    // Whether a modifier is currently pressed
    bool isModifierPressed() const;

    void pressModifier(int modifierKey);
    void pressKey(int key, bool pressed);

    void releaseModifiers();

private:
    // Send a key over fake_input, requires internal map to support this key
    void sendFakeKeyboardKey(int key, bool pressed);
    void updateModifierStateToUI();

    FakeInput *m_ext = nullptr;
    QSet<int> m_toggledModifierKeys;
};
