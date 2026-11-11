/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kcmutils as KCM
import org.kde.ki18n
import org.kde.kirigami as Kirigami

KCM.ScrollViewKCM {
    id: root

    KI18nContext {
        id: _tr
        translationDomain: "kcm_plasmakeyboard"
    }

    property int editingIndex: -1

    function openRuleDialog(index) {
        editingIndex = index;
        const rule = index < 0 ? null : kcm.rulesModel.ruleAt(index);
        abbreviationField.text = rule ? rule.abbreviation : "";
        expansionField.text = rule ? rule.expansion : "";
        ruleDialog.open();
    }

    actions: [
        Kirigami.Action {
            icon.name: "list-add"
            text: _tr.i18nc("@action:button", "Add shortcut")
            onTriggered: root.openRuleDialog(-1)
        }
    ]

    view: ListView {
        clip: true
        model: kcm.rulesModel
        spacing: Kirigami.Units.smallSpacing

        delegate: Kirigami.TitleSubtitleWithActions {
            id: delegate

            required property int index
            required property string abbreviation
            required property string expansion

            width: ListView.view.width

            title: delegate.abbreviation
            subtitle: delegate.expansion

            actions: [
                Kirigami.Action {
                    icon.name: "document-edit"
                    onTriggered: root.openRuleDialog(delegate.index)
                },
                Kirigami.Action {
                    icon.name: "edit-delete"
                    onTriggered: kcm.rulesModel.removeRule(delegate.index)
                }
            ]
        }
        Label {
            anchors.centerIn: parent
            visible: kcm.rulesModel.count === 0
            text: _tr.i18nc("@info:status", "No shortcuts configured")
            color: Kirigami.Theme.disabledTextColor
        }
    }

    Kirigami.Dialog {
        id: ruleDialog
        title: root.editingIndex < 0 ? _tr.i18nc("@title:window", "Add Shortcut") : _tr.i18nc("@title:window", "Edit Shortcut")
        standardButtons: Kirigami.Dialog.Save | Kirigami.Dialog.Cancel
        preferredWidth: Kirigami.Units.gridUnit * 20
        preferredHeight: Kirigami.Units.gridUnit * 15

        onAccepted: {
            console.warn("Accepted dialog with editingIndex:", root.editingIndex);
            if (root.editingIndex < 0) {
                kcm.rulesModel.addRule(abbreviationField.text, expansionField.text);
            } else {
                kcm.rulesModel.updateRule(root.editingIndex, abbreviationField.text, expansionField.text);
            }
        }

        Kirigami.FormLayout {
            TextField {
                id: abbreviationField
                Kirigami.FormData.label: _tr.i18nc("@label:textbox", "Abbreviation")
                placeholderText: _tr.i18nc("@info:placeholder", "For example :email")
            }

            TextArea {
                id: expansionField
                Kirigami.FormData.label: _tr.i18nc("@label:textbox", "Expansion")
                Layout.fillWidth: true
                placeholderText: _tr.i18nc("@info:placeholder", "Text to insert")
                wrapMode: TextEdit.Wrap
            }
        }
    }
}
