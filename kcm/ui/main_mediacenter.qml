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

    title: i18n("On-Screen Keyboard")
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
            description: {
                const layoutCount = kcm.plasmaKeyboardSettings.enabledKeyboardLayoutIds.length;
                if (layoutCount === 0) {
                    return i18n("No keyboard layouts selected, the default keyboard layout for the system will be used");
                }
                return i18ncp("%1 is the number of enabled keyboard layouts", "%1 keyboard layout selected", "%1 keyboard layouts selected", layoutCount);
            }
            font.pixelSize: Bigscreen.Units.headingFontPixelSize
            onClicked: layoutSelectorSidebar.open()
            KeyNavigation.down: soundOnKeypressButton
        }

        QQC2.Label {
            id: keyPressFeedbackLabel
            text: i18n("Key press feedback")
            font.pixelSize: Bigscreen.Units.headingFontPixelSize

            Layout.topMargin: Kirigami.Units.gridUnit
            Layout.bottomMargin: Kirigami.Units.gridUnit
        }

        Bigscreen.SwitchDelegate {
            id: soundOnKeypressButton
            text: i18nc("This is a noun", "Sound")
            description: i18n("A sound will play when a key is pressed")
            KeyNavigation.up: changeLayoutsButton
            KeyNavigation.down: vibrationOnKeypressButton

            checked: kcm.plasmaKeyboardSettings.soundEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.soundEnabled = checked
        }

        Bigscreen.SwitchDelegate {
            id: vibrationOnKeypressButton
            text: i18n("Vibration")
            description: i18n("If supported, the device will vibrate when a key is pressed")
            KeyNavigation.up: soundOnKeypressButton
            KeyNavigation.down: autoCapitalizationButton

            checked: kcm.plasmaKeyboardSettings.vibrationEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.vibrationEnabled = checked
        }

        QQC2.Label {
            id: generalLabel
            text: i18n("General")
            font.pixelSize: Bigscreen.Units.headingFontPixelSize

            Layout.topMargin: Kirigami.Units.gridUnit
            Layout.bottomMargin: Kirigami.Units.gridUnit
        }

        Bigscreen.SwitchDelegate {
            id: autoCapitalizationButton
            text: i18n("Auto-capitalization")
            description: i18n("Automatically capitalize the first letter of sentences")
            KeyNavigation.up: vibrationOnKeypressButton
            KeyNavigation.down: altCharsPopupButton

            checked: kcm.plasmaKeyboardSettings.autoCapitalizationEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.autoCapitalizationEnabled = checked
        }

        QQC2.Label {
            id: altCharsLabel
            text: i18nc("'Alternate' is an adjective here", "Alternate characters")
            font.pixelSize: Bigscreen.Units.headingFontPixelSize

            Layout.topMargin: Kirigami.Units.gridUnit
            Layout.bottomMargin: Kirigami.Units.gridUnit
        }

        Bigscreen.SwitchDelegate {
            id: altCharsPopupButton
            text: i18n("Show overlay when holding a key")
            description: i18n("Long-pressing a key on a physical keyboard will show an overlay with alternate versions of the selected character, if available")
            KeyNavigation.up: autoCapitalizationButton
            KeyNavigation.down: altCharsHoldDelayButton

            checked: kcm.plasmaKeyboardSettings.diacriticsPopupEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.diacriticsPopupEnabled = checked
        }

        Bigscreen.ButtonDelegate {
            id: altCharsHoldDelayButton
            text: i18nc("How long a key must be held before triggering an action", "Hold delay")
            description: i18n("How long a key must be held before the overlay shows")
            enabled: altCharsPopupButton.checked
            KeyNavigation.up: altCharsPopupButton
            trailing: QQC2.Label {
                text: i18np("%1 millisecond", "%1 milliseconds", kcm.plasmaKeyboardSettings.diacriticsHoldThresholdMs)
                font.pixelSize: Bigscreen.Units.defaultFontPixelSize
                bottomPadding: Kirigami.Units.largeSpacing
                rightPadding: Kirigami.Units.largeSpacing
            }
            onClicked: altCharsHoldDelayDialog.open()

            Bigscreen.Dialog {
                id: altCharsHoldDelayDialog
                title: i18nc("How long a key must be held before triggering an action", "Hold delay")
                standardButtons: Bigscreen.Dialog.Save | Bigscreen.Dialog.Cancel

                onOpened: scaleSlider.forceActiveFocus()
                onClosed: altCharsHoldDelayButton.forceActiveFocus()
                onAccepted: kcm.plasmaKeyboardSettings.diacriticsHoldThresholdMs = newValue

                property real newValue

                contentItem: Bigscreen.ButtonDelegate {
                    Keys.onRightPressed: scaleSlider.increase()
                    Keys.onLeftPressed: scaleSlider.decrease()
                    KeyNavigation.down: altCharsHoldDelayDialog.footer

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.Slider {
                            id: scaleSlider
                            Layout.fillWidth: true
                            from: 100
                            to: 1500
                            stepSize: 100
                            value: kcm.plasmaKeyboardSettings.diacriticsHoldThresholdMs
                            snapMode: QQC2.Slider.SnapAlways

                            onValueChanged: altCharsHoldDelayDialog.newValue = value
                        }

                        QQC2.Label {
                            id: altCharsHoldDelaySlider
                            text: i18np("%1 millisecond", "%1 milliseconds", altCharsHoldDelayDialog.newValue)
                            font.pixelSize: Bigscreen.Units.defaultFontPixelSize
                        }
                    }
                }
            }
        }

        KeyboardLayoutSelectorSidebar {
            id: layoutSelectorSidebar
            onClosed: changeLayoutsButton.forceActiveFocus()
        }
    }
}
