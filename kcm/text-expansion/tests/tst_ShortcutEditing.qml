/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtTest
import "../ui"

Item {
    id: root
    width: 600
    height: 600

    property alias kcm: mockKcm

    QtObject {
        id: mockKcm
        property bool editorDirty: false
        property var rulesModel: mockModel
        signal applyEditor()
    }

    QtObject {
        id: mockModel
        property var rules: []
        readonly property int count: rules.length

        function ruleAt(row) {
            return rules[row];
        }

        function addRule(abbreviation, expansion) {
            abbreviation = abbreviation.trim();
            if (rules.some(rule => rule.abbreviation === abbreviation)) {
                return false;
            }
            rules = [...rules, {abbreviation, expansion}];
            return true;
        }

        function updateRule(row, abbreviation, expansion) {
            abbreviation = abbreviation.trim();
            if (rules.some((rule, index) => index !== row && rule.abbreviation === abbreviation)) {
                return false;
            }
            const updated = [...rules];
            updated[row] = {abbreviation, expansion};
            rules = updated;
            return true;
        }
    }

    Component {
        id: pageComponent
        RulePage {
            width: 600
            height: 600
        }
    }

    TestCase {
        name: "ShortcutEditing"
        when: windowShown

        function init() {
            mockKcm.editorDirty = false;
            mockModel.rules = [];
        }

        function test_addAndApplyAgain() {
            const page = createTemporaryObject(pageComponent, root, {
                editingIndex: -1, originalAbbreviation: "", initialExpansion: ""
            });
            verify(!!page, "Component exists");
            const abbreviation = findChild(page, "abbreviationField");
            const expansion = findChild(page, "expansionField");
            verify(!!abbreviation, "Object exists");
            verify(!!expansion, "Object exists");

            abbreviation.text = " :email ";
            expansion.text = "hello@example.com";
            compare(mockKcm.editorDirty, true);
            compare(mockModel.count, 0);
            mockKcm.applyEditor();
            compare(mockKcm.editorDirty, false);
            compare(mockModel.count, 1);
            compare(mockModel.ruleAt(0).abbreviation, ":email");

            expansion.text = "updated@example.com";
            compare(mockKcm.editorDirty, true);
            mockKcm.applyEditor();
            compare(mockKcm.editorDirty, false);
            compare(mockModel.count, 1);
            compare(mockModel.ruleAt(0).expansion, "updated@example.com");
        }

        function test_invalidDraftAndCorrection() {
            mockModel.rules = [{abbreviation: ":email", expansion: "existing"}];
            const page = createTemporaryObject(pageComponent, root, {
                editingIndex: -1, originalAbbreviation: "", initialExpansion: ""
            });
            verify(!!page, "Component exists");
            const abbreviation = findChild(page, "abbreviationField");
            const expansion = findChild(page, "expansionField");
            verify(!!abbreviation, "Object exists");
            verify(!!expansion, "Object exists");

            abbreviation.text = ":email";
            mockKcm.applyEditor();
            verify(page.errorMessage !== "");
            compare(mockKcm.editorDirty, true);
            expansion.text = "new value";
            compare(page.errorMessage, "");
            mockKcm.applyEditor();
            verify(page.errorMessage !== "");
            compare(mockModel.count, 1);

            abbreviation.text = ":other";
            compare(page.errorMessage, "");
            mockKcm.applyEditor();
            compare(mockKcm.editorDirty, false);
            compare(mockModel.count, 2);
        }

        function test_editIsDraftUntilApplied() {
            mockModel.rules = [{abbreviation: ":one", expansion: "original"}];
            const page = createTemporaryObject(pageComponent, root, {
                editingIndex: 0, originalAbbreviation: ":one", initialExpansion: "original"
            });
            verify(!!page, "Component exists");
            const expansion = findChild(page, "expansionField");
            verify(!!expansion, "Object exists");
            expansion.text = "changed";
            compare(mockKcm.editorDirty, true);
            compare(mockModel.ruleAt(0).expansion, "original");
            expansion.text = "original";
            compare(mockKcm.editorDirty, false);
            compare(mockModel.ruleAt(0).expansion, "original");
        }

        function test_editAfterListChanges() {
            mockModel.rules = [
                {abbreviation: ":first", expansion: "first"},
                {abbreviation: ":second", expansion: "second"}
            ];
            const page = createTemporaryObject(pageComponent, root, {
                editingIndex: 1, originalAbbreviation: ":second", initialExpansion: "second"
            });
            verify(!!page, "Component exists");
            const expansion = findChild(page, "expansionField");
            verify(!!expansion, "Object exists");
            expansion.text = "updated";
            mockModel.rules = [mockModel.ruleAt(1)];
            mockKcm.applyEditor();
            compare(mockKcm.editorDirty, false);
            compare(mockModel.ruleAt(0).expansion, "updated");

            expansion.text = "another update";
            mockModel.rules = [];
            mockKcm.applyEditor();
            verify(page.errorMessage !== "");
            compare(mockKcm.editorDirty, true);
            compare(mockModel.count, 0);
        }
    }
}
