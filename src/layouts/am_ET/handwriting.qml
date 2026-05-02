/*
    SPDX-FileCopyrightText: 2026 Abenezer Wesenseged <wseged@proton.me>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Components

KeyboardLayout {
    function createInputMethod() {
        return Qt.createQmlObject('import QtQuick; import QtQuick.VirtualKeyboard.Plugins; HandwritingInputMethod {}', parent);
    }
    sharedLayouts: ['symbols']

    KeyboardRow {
        KeyboardColumn {
            Layout.preferredWidth: 1
            InputModeKey {}
            ChangeLanguageKey {
                visible: true
            }
            ShiftKey {}
            HandwritingModeKey {}
        }
        KeyboardColumn {
            Layout.preferredWidth: 8
            TraceInputKey {
                objectName: "hwrInputArea"
                patternRecognitionMode: InputEngine.PatternRecognitionMode.Handwriting
            }
        }
        KeyboardColumn {
            Layout.preferredWidth: 1
            Key {
                key: Qt.Key_Period
                text: "."
                alternativeKeys: "<>()/\\\"'=+-_:;,.?! "
                smallText: "!?"
                smallTextVisible: true
                highlighted: true
            }
            HideKeyboardKey {
                visible: true
            }
            BackspaceKey {}
            EnterKey {}
        }
    }
}
