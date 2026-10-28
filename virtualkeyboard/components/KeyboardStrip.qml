/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard

/*!
 * Strip above the keys, holding quick actions and word candidates from input methods.
 */
Rectangle {
    id: root

    property var inputEngine: VirtualKeyboard.inputEngine
    property bool navigationModeActive: false

    // Horizontal margins
    property real horizontalInset: 0

    readonly property real candidateHorizontalPadding: Math.round(28 * BreezeConstants.scaleHint)
    readonly property int candidateCount: candidateView.count
    readonly property bool hasNavigationSelection: selectedAction !== null || candidateView.currentIndex >= 0
    readonly property bool hasCandidates: inputEngine && inputEngine.wordCandidateListVisibleHint

    property list<Kirigami.Action> leadingActions: [
        Kirigami.Action {
            icon.name: "edit-paste-symbolic"
            text: i18nc("@action:button", "Paste")
            enabled: root.inputEngine && root.inputEngine.canPaste
            visible: !root.hasCandidates
            onTriggered: root.inputEngine.paste()
        }
    ]
    property list<Kirigami.Action> trailingActions: [] // TODO

    property Kirigami.Action selectedAction: null
    readonly property var currentNavigationItem: selectedAction ?? candidateView.currentIndex

    implicitHeight: Math.round(100 * BreezeConstants.scaleHint)
    color: BreezeConstants.selectionListBackgroundColor

    function resetNavigation(): void {
        selectNavigationItem(null);
    }

    function isActionAvailable(action: Kirigami.Action): bool {
        return action.visible && action.enabled;
    }

    function navigationItems(includedAction: Kirigami.Action): list<Kirigami.Action> {
        const available = action => action === includedAction || isActionAvailable(action);
        const candidates = Array.from({ length: candidateCount }, (_, i) => i);
        const leadingItems = Array.from(leadingActions).filter(available);
        const trailingItems = Array.from(trailingActions).filter(available);
        return [...leadingItems, ...candidates, ...trailingItems];
    }

    function selectNavigationItem(item): void {
        const isCandidate = typeof item === "number";
        selectedAction = isCandidate ? null : (item ?? null);
        candidateView.currentIndex = isCandidate ? item : -1;
        if (isCandidate) {
            candidateView.positionViewAtIndex(item, ListView.Contain);
        }
    }

    function handleActionAvailabilityChanged(action): void {
        if (action !== selectedAction || isActionAvailable(action)) {
            return;
        }
        // Move the navigation selection to neighbour if the action can no longer be used
        const items = navigationItems(action);
        const index = items.indexOf(action);
        selectNavigationItem(navigationModeActive ? (items[index - 1] ?? items[index + 1]) : null);
    }

    function ensureNavigationSelection(fromRight): bool {
        const items = navigationItems();
        if (!items.includes(currentNavigationItem)) {
            selectNavigationItem(fromRight ? items[items.length - 1] : (candidateCount > 0 ? 0 : items[0]));
        }
        return hasNavigationSelection;
    }

    function moveSelection(delta): bool {
        if (!ensureNavigationSelection(delta < 0)) {
            return false;
        }
        const items = navigationItems();
        const next = items[items.indexOf(currentNavigationItem) + delta];
        if (next === undefined) {
            return false;
        }
        selectNavigationItem(next);
        return true;
    }

    function activateCurrent(): bool {
        if (!ensureNavigationSelection(false)) {
            return false;
        }

        if (selectedAction) {
            Feedback.play(Feedback.Press);
            selectedAction.trigger();
            return true;
        }

        Feedback.play(Feedback.SelectionCommit);
        candidateView.model.selectItem(candidateView.currentIndex);
        return true;
    }

    RowLayout {
        anchors {
            fill: parent
            leftMargin: root.horizontalInset
            rightMargin: root.horizontalInset
        }
        spacing: 0

        Repeater {
            model: root.leadingActions

            delegate: ActionButton {
                required property Kirigami.Action modelData
                action: modelData
                navigationActive: root.navigationModeActive && root.selectedAction === modelData
                onEnabledChanged: root.handleActionAvailabilityChanged(modelData)
                onVisibleChanged: root.handleActionAvailabilityChanged(modelData)
            }
        }

        Separator {
            visible: root.hasCandidates && Array.from(root.leadingActions).some(action => action.visible)
        }

        ListView {
            id: candidateView
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: ListView.Horizontal
            spacing: 0
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            model: root.inputEngine ? root.inputEngine.wordCandidateListModel : null

            delegate: Item {
                required property int index
                required property string display

                width: Math.round(candidateLabel.implicitWidth + root.candidateHorizontalPadding * 2)
                height: candidateView.height
                readonly property bool navigationActive: root.navigationModeActive && candidateView.currentIndex === index
                readonly property bool pressed: candidateTapHandler.pressed

                Rectangle {
                    anchors {
                        fill: parent
                        // Avoid separator
                        leftMargin: candidateSeparator.visible ? candidateSeparator.width : 0
                    }
                    color: (navigationActive || pressed) ? BreezeConstants.popupHighlightColor : "transparent"
                }

                QQC2.Label {
                    id: candidateLabel
                    anchors.left: parent.left
                    anchors.leftMargin: root.candidateHorizontalPadding
                    anchors.verticalCenter: parent.verticalCenter
                    text: display
                    color: BreezeConstants.selectionListTextColor
                    opacity: navigationActive || pressed ? 1 : 0.9
                    font.family: BreezeConstants.fontFamily
                    font.weight: navigationActive || pressed ? Font.Medium : Font.Light
                    font.pixelSize: Math.round(44 * BreezeConstants.scaleHint)
                }

                Separator {
                    id: candidateSeparator
                    visible: index > 0
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    z: 1
                }

                TapHandler {
                    id: candidateTapHandler

                    onPressedChanged: {
                        if (pressed) {
                            Feedback.play(Feedback.Press);
                        }
                    }

                    onTapped: {
                        root.selectedAction = null;
                        candidateView.currentIndex = index;
                        Feedback.play(Feedback.SelectionCommit);
                        candidateView.model.selectItem(index);
                    }
                }
            }
        }

        Separator {
            visible: root.hasCandidates && Array.from(root.trailingActions).some(action => action.visible)
        }

        Repeater {
            model: root.trailingActions

            delegate: ActionButton {
                required property Kirigami.Action modelData
                action: modelData
                navigationActive: root.navigationModeActive && root.selectedAction === modelData
                onEnabledChanged: root.handleActionAvailabilityChanged(modelData)
                onVisibleChanged: root.handleActionAvailabilityChanged(modelData)
            }
        }
    }

    component Separator: Rectangle {
        implicitWidth: Math.round(4 * BreezeConstants.scaleHint)
        implicitHeight: Math.round(36 * BreezeConstants.scaleHint)
        Layout.alignment: Qt.AlignVCenter
        radius: width / 2
        color: BreezeConstants.selectionListSeparatorColor
    }

    component ActionButton: Item {
        id: actionButton

        required property Kirigami.Action action
        property bool navigationActive: false

        Layout.fillHeight: true
        Layout.preferredWidth: height
        visible: action.visible
        enabled: action.enabled
        opacity: enabled ? 1 : 0.4

        Accessible.role: Accessible.Button
        Accessible.name: action.text
        Accessible.onPressAction: action.trigger()

        Rectangle {
            anchors.fill: parent
            color: (actionButton.navigationActive || actionTapHandler.pressed) ? BreezeConstants.popupHighlightColor : "transparent"
        }

        Kirigami.Icon {
            anchors.centerIn: parent
            implicitWidth: Math.round(48 * BreezeConstants.scaleHint)
            implicitHeight: implicitWidth
            source: actionButton.action.icon.name
            color: BreezeConstants.selectionListTextColor
            isMask: true
        }

        TapHandler {
            id: actionTapHandler

            onPressedChanged: {
                if (pressed) {
                    Feedback.play(Feedback.Press);
                }
            }

            onTapped: actionButton.action.trigger()
        }
    }
}
