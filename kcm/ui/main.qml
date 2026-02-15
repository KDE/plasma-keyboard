/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

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

            checked: kcm.soundEnabled
            onCheckedChanged: {
                kcm.soundEnabled = checked;
                checked = Qt.binding(() => kcm.soundEnabled)
            }
        }

        QQC2.CheckBox {
            id: vibrationEnabled
            text: i18n("Vibration")

            checked: kcm.vibrationEnabled
            onCheckedChanged: {
                kcm.vibrationEnabled = checked;
                checked = Qt.binding(() => kcm.vibrationEnabled)
            }
        }

        QQC2.CheckBox {
            id: keyboardNavigationEnabled
            Kirigami.FormData.label: i18n("General:")
            text: i18n("Keyboard navigation")

            checked: kcm.keyboardNavigationEnabled
            onCheckedChanged: {
                kcm.keyboardNavigationEnabled = checked;
                checked = Qt.binding(() => kcm.keyboardNavigationEnabled)
            }
        }

        QQC2.CheckBox {
            id: autoCapitalizationEnabled
            text: i18n("Auto capitalization")

            checked: kcm.autoCapitalizationEnabled
            onCheckedChanged: {
                kcm.autoCapitalizationEnabled = checked;
                checked = Qt.binding(() => kcm.autoCapitalizationEnabled)
            }
        }
    }
}
