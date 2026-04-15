// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard

// Space key that labels itself with the current input language.

Key {
    key: Qt.Key_Space
    text: " "
    displayText: VirtualKeyboard.inputEngine ? Qt.locale(VirtualKeyboard.inputEngine.locale).nativeLanguageName : ""
    noModifier: true
    textPixelSize: 24 * scaleHint
}
