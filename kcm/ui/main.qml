/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM

KCM.ScrollViewKCM {
    id: root

    view: LocaleSelectorListView {
        id: list

        Kirigami.Separator {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }

    footer: Kirigami.FormLayout {
        id: formLayout

        QQC2.CheckBox {
            id: soundsEnabled
            Kirigami.FormData.label: i18n("Key press feedback:")
            text: i18n("Sound")

            checked: kcm.plasmaKeyboardSettings.soundEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.soundEnabled = checked
        }

        QQC2.CheckBox {
            id: vibrationEnabled
            text: i18n("Vibration")

            checked: kcm.plasmaKeyboardSettings.vibrationEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.vibrationEnabled = checked
        }

        QQC2.CheckBox {
            id: keyboardNavigationEnabled
            Kirigami.FormData.label: i18n("General:")
            text: i18n("Keyboard navigation")

            checked: kcm.plasmaKeyboardSettings.keyboardNavigationEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.keyboardNavigationEnabled = checked
        }

        QQC2.CheckBox {
            id: autoCapitalizationEnabled
            text: i18n("Auto-capitalization")

            checked: kcm.plasmaKeyboardSettings.autoCapitalizationEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.autoCapitalizationEnabled = checked
        }

        QQC2.CheckBox {
            id: diacriticsCheckbox
            Kirigami.FormData.label: i18n("Alternate characters:")
            text: i18n("Show popup when holding a key")

            checked: kcm.plasmaKeyboardSettings.diacriticsPopupEnabled
            onCheckedChanged: kcm.plasmaKeyboardSettings.diacriticsPopupEnabled = checked
        }

        QQC2.SpinBox {
            id: diacriticsDelaySpinBox
            Kirigami.FormData.label: i18n("Hold delay:")
            from: 100
            to: 1500
            stepSize: 50

            enabled: diacriticsCheckbox.checked
            value: kcm.plasmaKeyboardSettings.diacriticsHoldThresholdMs

            // Include the `milliseconds` suffix in the spinbox instead of the label
            textFromValue: function (value) {
                return value + " " + i18n("milliseconds");
            }

            // Parse the integer value from the spinbox text, ignoring the suffix
            valueFromText: function (text) {
                let number = parseInt(text);
                if (isNaN(number)) {
                    return kcm.plasmaKeyboardSettings.diacriticsHoldThresholdMs; // Fallback to current value if parsing fails
                }
                return number;
            }

            onValueChanged: kcm.plasmaKeyboardSettings.diacriticsHoldThresholdMs = value
        }
    }
}
