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

KCM.SimpleKCM {
    id: root

    Kirigami.FormLayout {
        id: formLayout

        width: flickable.width

        QQC2.Button {
            Kirigami.FormData.label: i18n("Keyboard layouts:")
            text: i18n("Configure…")
            icon.name: "languages"
            onClicked: kcm.push(layoutPage);

            Kirigami.ScrollablePage {
                id: layoutPage
                title: i18n("Keyboard Layouts")

                KeyboardLayoutSelectorListView {}
            }
        }

        QQC2.CheckBox {
            id: soundsEnabled
            Kirigami.FormData.label: i18n("Key press feedback:")
            text: i18n("Sound")

            checked: kcm.soundEnabled
            onCheckedChanged: {
                kcm.soundEnabled = checked;
                checked = Qt.binding(() => kcm.soundEnabled);
            }
        }

        QQC2.CheckBox {
            id: vibrationEnabled
            text: i18n("Vibration")

            checked: kcm.vibrationEnabled
            onCheckedChanged: {
                kcm.vibrationEnabled = checked;
                checked = Qt.binding(() => kcm.vibrationEnabled);
            }
        }

        QQC2.CheckBox {
            id: keyboardNavigationEnabled
            Kirigami.FormData.label: i18n("General:")
            text: i18n("Keyboard navigation")

            checked: kcm.keyboardNavigationEnabled
            onCheckedChanged: {
                kcm.keyboardNavigationEnabled = checked;
                checked = Qt.binding(() => kcm.keyboardNavigationEnabled);
            }
        }

        QQC2.CheckBox {
            id: diacriticsCheckbox
            Kirigami.FormData.label: i18n("Alternate characters:")
            text: i18n("Show popup when holding a key")

            checked: kcm.diacriticsPopupEnabled
            onCheckedChanged: {
                kcm.diacriticsPopupEnabled = checked;
                checked = Qt.binding(() => kcm.diacriticsPopupEnabled);
            }
        }

        QQC2.SpinBox {
            id: diacriticsDelaySpinBox
            Kirigami.FormData.label: i18n("Hold delay:")
            from: 100
            to: 1500
            stepSize: 50

            enabled: diacriticsCheckbox.checked
            value: kcm.diacriticsHoldThresholdMs

            // Include the `milliseconds` suffix in the spinbox instead of the label
            textFromValue: function (value) {
                return value + " " + i18n("milliseconds");
            }

            // Parse the integer value from the spinbox text, ignoring the suffix
            valueFromText: function (text) {
                let number = parseInt(text);
                if (isNaN(number)) {
                    return kcm.diacriticsHoldThresholdMs; // Fallback to current value if parsing fails
                }
                return number;
            }

            onValueChanged: {
                kcm.diacriticsHoldThresholdMs = value;
                value = Qt.binding(() => kcm.diacriticsHoldThresholdMs);
            }
        }
    }
}
