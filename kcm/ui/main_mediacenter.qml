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
            changeLanguagesButton.forceActiveFocus()
        }
    }

    ColumnLayout {
        id: column
        KeyNavigation.left: keyboardSettingsView.KeyNavigation.left
        spacing: 0

        Bigscreen.ButtonDelegate {
            id: changeLanguagesButton
            text: i18n("Languages")
            description: {
                if (kcm.enabledLocales.length == 0) {
                    return i18n("No languages selected, the default keyboard layout for the system will be used")
                }
                return i18ncp("%1 is the number of enabled locales", "%1 language selected", "%1 languages selected", kcm.enabledLocales.length)
            }
            font.pixelSize: Bigscreen.Units.headingFontPixelSize
            onClicked: localeSelectorSidebar.open()
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
            KeyNavigation.down: vibrationOnKeypressButton

            checked: kcm.soundEnabled
            onCheckedChanged: {
                kcm.soundEnabled = checked;
                checked = Qt.binding(() => kcm.soundEnabled);
            }
        }

        Bigscreen.SwitchDelegate {
            id: vibrationOnKeypressButton
            text: i18n("Vibration")
            description: i18n("If supported, the device will vibrate when a key is pressed")
            KeyNavigation.down: autoCapitalizationButton

            checked: kcm.vibrationEnabled
            onCheckedChanged: {
                kcm.vibrationEnabled = checked;
                checked = Qt.binding(() => kcm.vibrationEnabled);
            }
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
            description: i18n("Automatically capitalize the beginning of sentences and proper nouns")
            KeyNavigation.down: altCharsPopupButton

            checked: kcm.autoCapitalizationEnabled
            onCheckedChanged: {
                kcm.autoCapitalizationEnabled = checked;
                checked = Qt.binding(() => kcm.autoCapitalizationEnabled);
            }
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
            KeyNavigation.down: altCharsHoldDelayButton

            checked: kcm.diacriticsPopupEnabled
            onCheckedChanged: {
                kcm.diacriticsPopupEnabled = checked;
                checked = Qt.binding(() => kcm.diacriticsPopupEnabled);
            }
        }

        Bigscreen.ButtonDelegate {
            id: altCharsHoldDelayButton
            text: i18nc("How long a key must be held before triggering an action", "Hold delay")
            description: i18n("How long a key must be held before the overlay shows")
            enabled: altCharsPopupButton.checked
            trailing: QQC2.Label {
                text: i18np("%1 millisecond", "%1 milliseconds", kcm.diacriticsHoldThresholdMs)
                font.pixelSize: Bigscreen.Units.defaultFontPixelSize
                bottomPadding: Kirigami.Units.largeSpacing
                rightPadding: Kirigami.Units.largeSpacing
            }
            onClicked: altCharsHoldDelayDialog.open()

            Bigscreen.Dialog {
                id: altCharsHoldDelayDialog
                title: i18nc("How long a key must be held before triggering an action", "Hold delay")
                standardButtons: Bigscreen.Dialog.Save | Bigscreen.Dialog.Cancel

                onOpened: altCharsHoldDelayField.forceActiveFocus()
                onClosed: altCharsHoldDelayButton.forceActiveFocus()
                onAccepted: kcm.diacriticsHoldThresholdMs = newValue

                property real newValue

                contentItem: Bigscreen.ButtonDelegate {
                    Keys.onRightPressed: {
                        scaleSlider.increase();
                    }

                    Keys.onLeftPressed: {
                        scaleSlider.decrease()
                    }
                    KeyNavigation.down: altCharsHoldDelayDialog.footer

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.Slider {
                            Layout.fillWidth: true
                            id: scaleSlider
                            from: 100
                            to: 1500
                            stepSize: 100
                            value: kcm.diacriticsHoldThresholdMs
                            snapMode: QQC2.Slider.SnapAlways

                            onValueChanged: altCharsHoldDelayDialog.newValue = value;
                        }

                        QQC2.Label {
                            id: altCharsHoldDelaySlider
                            text: i18n("%1 milliseconds", altCharsHoldDelayDialog.newValue)
                            font.pixelSize: Bigscreen.Units.defaultFontPixelSize
                        }
                    }
                }
            }

        }

        LocaleSelectorSidebar {
            id: localeSelectorSidebar
            onClosed: changeLanguagesButton.forceActiveFocus()
        }
    }
}
