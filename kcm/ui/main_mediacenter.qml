/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kcmutils as KCM
import org.kde.kirigami as Kirigami
import org.kde.bigscreen as Bigscreen

KCM.SimpleKCM {
    id: keyboardSettingsView

    title: ("On-Screen Keyboard")
    background: null

    leftPadding: Kirigami.Units.smallSpacing
    topPadding: Kirigami.Units.smallSpacing
    rightPadding: Kirigami.Units.smallSpacing
    bottomPadding: Kirigami.Units.smallSpacing

    onActiveFocusChanged: {
        if (activeFocus) {
            changeLayoutsButton.forceActiveFocus()
        }
    }

    ColumnLayout {
        id: column
        KeyNavigation.left: keyboardSettingsView.KeyNavigation.left
        spacing: 0

        Bigscreen.ButtonDelegate {
            id: changeLayoutsButton
            text: i18n("Keyboard Layouts")
            onClicked: layoutSelectorSidebar.open()
            KeyNavigation.down: soundOnKeypressButton
        }

        QQC2.Label {
            id: keyPressFeedbackLabel
            text: i18n("Key press feedback")
            font.pixelSize: 22
            font.weight: Font.Normal
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }

        Bigscreen.SwitchDelegate {
            id: soundOnKeypressButton
            text: "Sound"
            KeyNavigation.up: changeLayoutsButton
            KeyNavigation.down: vibrationOnKeypressButton

            checked: kcm.soundEnabled
            onCheckedChanged: {
                kcm.soundEnabled = checked;
                checked = Qt.binding(() => kcm.soundEnabled);
            }
        }

        Bigscreen.SwitchDelegate {
            id: vibrationOnKeypressButton
            text: "Vibration"
            KeyNavigation.up: soundOnKeypressButton

            checked: kcm.vibrationEnabled
            onCheckedChanged: {
                kcm.vibrationEnabled = checked;
                checked = Qt.binding(() => kcm.vibrationEnabled);
            }
        }

        QQC2.Label {
            id: generalLabel
            text: i18n("General")
            font.pixelSize: 22
            font.weight: Font.Normal
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }

        KeyboardLayoutSelectorSidebar {
            id: layoutSelectorSidebar
            onClosed: changeLayoutsButton.forceActiveFocus()
        }
    }
}
