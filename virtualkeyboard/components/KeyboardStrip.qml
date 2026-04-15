/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard

// Word candidate strip shown by input methods when there are suggestions.

Rectangle {
    id: root

    property var inputEngine: VirtualKeyboard.inputEngine
    property bool navigationModeActive: false
    readonly property real candidateHorizontalPadding: Math.round(28 * BreezeConstants.scaleHint)
    readonly property int candidateCount: candidateView.count
    readonly property bool hasNavigationSelection: candidateView.currentIndex >= 0
    readonly property bool hasCandidates: inputEngine && inputEngine.wordCandidateListVisibleHint

    visible: hasCandidates
    implicitHeight: Math.round(100 * BreezeConstants.scaleHint)
    color: BreezeConstants.selectionListBackgroundColor

    function resetNavigation() {
        candidateView.currentIndex = -1;
    }

    function ensureNavigationSelection(fromRight) {
        if (candidateCount <= 0) {
            candidateView.currentIndex = -1;
            return false;
        }
        if (candidateView.currentIndex === -1) {
            candidateView.currentIndex = fromRight ? candidateCount - 1 : 0;
        }
        candidateView.positionViewAtIndex(candidateView.currentIndex, ListView.Contain);
        return true;
    }

    function moveSelection(delta) {
        if (!ensureNavigationSelection(delta < 0)) {
            return false;
        }

        const nextIndex = candidateView.currentIndex + delta;
        if (nextIndex < 0) {
            return false;
        }
        if (nextIndex >= candidateCount) {
            return false;
        }

        candidateView.currentIndex = nextIndex;
        candidateView.positionViewAtIndex(candidateView.currentIndex, ListView.Contain);
        return true;
    }

    function activateCurrent() {
        if (!ensureNavigationSelection(false)) {
            return false;
        }

        Feedback.play(Feedback.SelectionCommit);
        candidateView.model.selectItem(candidateView.currentIndex);
        return true;
    }

    ListView {
        id: candidateView
        anchors.fill: parent
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
                    leftMargin: index > 0 ? Math.round(4 * BreezeConstants.scaleHint) : 0
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

            Rectangle {
                visible: index > 0
                width: Math.round(4 * BreezeConstants.scaleHint)
                height: Math.round(36 * BreezeConstants.scaleHint)
                radius: width / 2
                color: BreezeConstants.selectionListSeparatorColor
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
                    candidateView.currentIndex = index;
                    Feedback.play(Feedback.SelectionCommit);
                    candidateView.model.selectItem(index);
                }
            }
        }
    }
}
