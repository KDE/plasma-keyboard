/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.kcmutils as KCM
import org.kde.ki18n
import org.kde.kirigami as Kirigami

KCM.ScrollViewKCM {
    id: root

    KI18nContext {
        id: _tr
        translationDomain: "kcm_plasmakeyboard"
    }

    function openRulePage(index) {
        if (kcm.depth > 1) {
            kcm.pop();
        }
        const rule = index < 0 ? null : kcm.rulesModel.ruleAt(index);
        kcm.push("RulePage.qml", {
            editingIndex: index,
            originalAbbreviation: rule ? rule.abbreviation : "",
            initialExpansion: rule ? rule.expansion : ""
        });
    }

    Connections {
        target: kcm

        function onCurrentIndexChanged() {
            if (kcm.currentIndex === 0 && kcm.depth > 1) {
                kcm.pop();
            }
        }
    }

    actions: [
        Kirigami.Action {
            icon.name: "list-add"
            text: _tr.i18nc("@action:button", "Add shortcut")
            onTriggered: root.openRulePage(-1)
        }
    ]

    view: ListView {
        clip: true
        model: kcm.rulesModel
        spacing: Kirigami.Units.smallSpacing

        delegate: ItemDelegate {
            id: itemDelegate

            required property int index
            required property string abbreviation
            required property string expansion

            Kirigami.Theme.useAlternateBackgroundColor: true
            width: parent?.width

            onClicked: root.openRulePage(itemDelegate.index)

            contentItem: Kirigami.TitleSubtitleWithActions {
                title: itemDelegate.abbreviation
                subtitle: itemDelegate.expansion

                actions: [
                    Kirigami.Action {
                        icon.name: "document-edit"
                        onTriggered: root.openRulePage(itemDelegate.index)
                    },
                    Kirigami.Action {
                        icon.name: "edit-delete"
                        onTriggered: kcm.rulesModel.removeRule(itemDelegate.index)
                    }
                ]
            }
        }
        Label {
            anchors.centerIn: parent
            visible: kcm.rulesModel.count === 0
            text: _tr.i18nc("@info:status", "No shortcuts configured")
            color: Kirigami.Theme.disabledTextColor
        }
    }

}
