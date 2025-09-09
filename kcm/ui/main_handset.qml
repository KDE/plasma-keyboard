/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

KCM.SimpleKCM {
    id: root

    leftPadding: 0
    rightPadding: 0
    topPadding: Kirigami.Units.gridUnit
    bottomPadding: Kirigami.Units.gridUnit

    ColumnLayout {
        spacing: 0
        width: parent.width

        FormCard.FormCard {
            FormCard.FormTextFieldDelegate {
                label: i18n("Test the virtual keyboard…")
            }
        }

        FormCard.FormHeader {
            title: i18nc("@title:group", "Feedback")
        }

        FormCard.FormCard {
            FormCard.FormSwitchDelegate {
                id: soundsEnabled
                text: i18n("Sound")
                description: i18n("Whether to emit a sound on key press")

                checked: kcm.soundEnabled
                onCheckedChanged: {
                    kcm.soundEnabled = checked;
                    checked = Qt.binding(() => kcm.soundEnabled)
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormSwitchDelegate {
                id: vibrationEnabled
                text: i18n("Vibration")
                description: i18n("Whether to vibrate on key press")

                checked: kcm.vibrationEnabled
                onCheckedChanged: {
                    kcm.vibrationEnabled = checked;
                    checked = Qt.binding(() => kcm.vibrationEnabled)
                }
            }
        }

        FormCard.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing

            FormCard.FormButtonDelegate {
                id: languageList
                text: i18n("Languages")
                icon.name: 'languages'
                onClicked: kcm.push(localePage)

                Kirigami.ScrollablePage {
                    id: localePage
                    title: i18n("Keyboard Languages")

                    LocaleSelectorListView {}
                }
            }
        }

        FormCard.FormHeader {
            title: i18nc("@title:group", "General")
        }

        FormCard.FormCard {
            FormCard.FormSwitchDelegate {
                id: keyboardNavigationEnabled
                text: i18n("Keyboard navigation")
                description: i18n("Whether to use the arrow keys to navigate the keyboard")

                checked: kcm.keyboardNavigationEnabled
                onCheckedChanged: {
                    kcm.keyboardNavigationEnabled = checked;
                    checked = Qt.binding(() => kcm.keyboardNavigationEnabled)
                }
            }
        }
    }
}
