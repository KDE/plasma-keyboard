/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    id: root

    required property int editingIndex
    required property string originalAbbreviation
    required property string initialExpansion
    property string errorMessage: ""
    property bool ready: false
    readonly property bool dirty: abbreviationField.text !== originalAbbreviation || expansionField.text !== initialExpansion

    title: editingIndex < 0 ? _tr.i18nc("@title:window", "Add Shortcut") : _tr.i18nc("@title:window", "Edit Shortcut")

    onDirtyChanged: {
        if (root.ready) {
            kcm.editorDirty = root.dirty;
            root.errorMessage = "";
        }
    }
    Component.onCompleted: root.ready = true

    Connections {
        target: kcm

        function onApplyEditor() {
            if (abbreviationField.text.trim() === "" || expansionField.text === "") {
                root.errorMessage = _tr.i18nc("@info", "Enter an abbreviation and expansion.");
                return;
            }

            let row = -1;
            if (root.editingIndex >= 0) {
                for (let i = 0; i < kcm.rulesModel.count; ++i) {
                    if (kcm.rulesModel.ruleAt(i).abbreviation === root.originalAbbreviation) {
                        row = i;
                        break;
                    }
                }
                if (row < 0) {
                    root.errorMessage = _tr.i18nc("@info", "This shortcut no longer exists.");
                    return;
                }
            }

            const saved = root.editingIndex < 0
                ? kcm.rulesModel.addRule(abbreviationField.text, expansionField.text)
                : kcm.rulesModel.updateRule(row, abbreviationField.text, expansionField.text);
            if (!saved) {
                root.errorMessage = _tr.i18nc("@info", "A shortcut with this abbreviation already exists.");
                return;
            }

            root.editingIndex = root.editingIndex < 0 ? kcm.rulesModel.count - 1 : row;
            abbreviationField.text = abbreviationField.text.trim();
            root.originalAbbreviation = abbreviationField.text;
            root.initialExpansion = expansionField.text;
            root.errorMessage = "";
            kcm.editorDirty = root.dirty;
        }
    }

    KI18nContext {
        id: _tr
        translationDomain: "kcm_plasmakeyboard"
    }

    ColumnLayout {
        spacing: Kirigami.Units.gridUnit

        Kirigami.InlineMessage {
            Layout.fillWidth: true
            visible: root.errorMessage !== ""
            type: Kirigami.MessageType.Error
            text: root.errorMessage
        }

        Kirigami.Form {
            Layout.fillWidth: true

            Kirigami.FormGroup {
                Kirigami.FormEntry {
                    title: _tr.i18nc("@label:textbox", "Abbreviation")
                    contentItem: TextField {
                        id: abbreviationField
                        objectName: "abbreviationField"
                        text: root.originalAbbreviation
                        placeholderText: _tr.i18nc("@info:placeholder", "For example :email")
                        Layout.fillWidth: true
                        onTextChanged: {
                            if (root.ready) {
                                root.errorMessage = "";
                            }
                        }
                    }
                }

                Kirigami.FormEntry {
                    title: _tr.i18nc("@label:textbox", "Expansion")
                    contentItem: TextArea {
                        id: expansionField
                        objectName: "expansionField"
                        text: root.initialExpansion
                        placeholderText: _tr.i18nc("@info:placeholder", "Text to insert")
                        wrapMode: TextEdit.Wrap

                        FontMetrics {
                            id: fontMetrics
                            font: expansionField.font
                        }

                        // Calculates the implicit height of the TextArea based on its content and
                        // font metrics.
                        // 
                        // This lets us start the TextArea with multiple lines in order to hint to
                        // the user that it can accept multiple lines of input, while still
                        // allowing the TextArea to grow as needed.
                        function calculateImplicitHeight(): int {
                            return Math.max(
                                (fontMetrics.lineSpacing * 2) + expansionField.topPadding + expansionField.bottomPadding,
                                contentHeight + expansionField.topPadding + expansionField.bottomPadding
                            );
                        }

                        implicitHeight: calculateImplicitHeight();

                        Layout.fillWidth: true

                        onTextChanged: {
                            if (root.ready) {
                                root.errorMessage = "";
                            }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter

            Label {
                text: _tr.i18nc("@label", "Variables")
                wrapMode: Text.Wrap
            }

            Label {
                text: _tr.i18nc("@info", "Expansion text supports variables:\n{date}, {time}, {datetime}")
                color: Kirigami.Theme.disabledTextColor
                wrapMode: Text.Wrap
            }
        }
    }
}
