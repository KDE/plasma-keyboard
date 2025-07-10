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

    header: Kirigami.FormLayout {
        id: formLayout

        QQC2.CheckBox {
            id: soundsEnabled
            Kirigami.FormData.label: i18n('General:')
            text: i18n('Key press sound')

            checked: kcm.soundEnabled
            onCheckedChanged: {
                kcm.soundEnabled = checked;
                checked = Qt.binding(() => kcm.soundEnabled)
            }
        }

        QQC2.CheckBox {
            id: vibrationEnabled
            text: i18n('Key press vibration')

            checked: kcm.vibrationEnabled
            onCheckedChanged: {
                kcm.vibrationEnabled = checked;
                checked = Qt.binding(() => kcm.vibrationEnabled)
            }
        }
    }

    view: LocaleSelectorListView {
        id: list

        headerPositioning: ListView.OverlayHeader
        header: Kirigami.InlineViewHeader {
            width: list.width
            text: i18nc("@title:view", "Keyboard Languages")

            topPadding: Kirigami.Units.largeSpacing
            bottomPadding: Kirigami.Units.largeSpacing
            leftPadding: Kirigami.Units.largeSpacing
            rightPadding: Kirigami.Units.largeSpacing
        }
    }
}
