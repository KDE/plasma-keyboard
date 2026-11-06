/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kcmutils as KCM
import org.kde.kirigami as Kirigami

KCM.ScrollViewKCM {
    id: root

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
            text: i18n("Add shortcut")
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
            text: i18n("No shortcuts configured")
            color: Kirigami.Theme.disabledTextColor
        }
    }

    Dialog {
        id: ruleDialog
        modal: true
        title: root.editingIndex < 0 ? i18n("Add Shortcut") : i18n("Edit Shortcut")
        standardButtons: Dialog.Save | Dialog.Cancel
        onAccepted: {
            console.warn("Accepted dialog with editingIndex:", root.editingIndex);
            if (root.editingIndex < 0) {
                kcm.rulesModel.addRule(abbreviationField.text, expansionField.text);
            } else {
                kcm.rulesModel.updateRule(root.editingIndex, abbreviationField.text, expansionField.text);
            }
        }

        contentItem: ColumnLayout {
            TextField {
                id: abbreviationField
                Layout.fillWidth: true
                placeholderText: i18n("Abbreviation, for example :email")
            }
            TextArea {
                id: expansionField
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                placeholderText: i18n("Text to insert")
                wrapMode: TextEdit.Wrap
            }
        }
    }
}
