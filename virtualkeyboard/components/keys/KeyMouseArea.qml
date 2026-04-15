// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

// Controller for tap/hold/swiping on keys.
// Implements key repeat, press-and-hold, listening to swipe positions, all
// while not taking exclusive grab (so multiple keys can be pressed at the
// same time).

MultiPointTouchArea {
    id: root

    /**
     * Key item that owns the pressed visual state and feedback behavior.
     */
    required property Item keyItem

    /**
     * Whether holding this key should repeatedly trigger repeatTriggered.
     */
    property bool repeatEnabled: keyItem.repeat

    /**
     * Delay before key repeat starts.
     */
    property int repeatDelay: 600

    /**
     * Interval used after key repeat starts.
     */
    property int repeatInterval: 50

    /**
     * Delay before pressAndHold is emitted.
     */
    property int pressAndHoldInterval: 800

    property bool __pressed: false
    property var __lastPoint: touchPoint

    signal pressStarted(var point)
    signal pressAndHold(var point)
    signal positionChanged(var point)
    signal releaseFinished(var point)
    signal cancelFinished()
    signal repeatTriggered()

    anchors.fill: parent
    z: 1
    minimumTouchPoints: 1
    maximumTouchPoints: 1
    mouseEnabled: true

    function stopRepeat() {
        repeatDelayTimer.stop();
        repeatTimer.stop();
    }

    function startPress(point) {
        if (root.__pressed) {
            cancelPress();
        }

        root.__pressed = true;
        root.__lastPoint = point;
        FlickKeyPrivate.keyPressStarted(keyItem);
        keyItem.pressedVisual = true;
        keyItem.playPressFeedback();
        root.pressStarted(point);
        pressAndHoldTimer.restart();
        if (root.repeatEnabled) {
            root.repeatTriggered();
            repeatDelayTimer.start();
        }
    }

    function updatePress(point) {
        if (!root.__pressed) {
            return;
        }

        root.__lastPoint = point;
        root.positionChanged(point);
    }

    function finishPress(point) {
        if (!root.__pressed) {
            return;
        }

        root.__pressed = false;
        root.__lastPoint = point;
        keyItem.pressedVisual = false;
        pressAndHoldTimer.stop();
        root.stopRepeat();
        root.releaseFinished(point);
    }

    function cancelPress() {
        if (!root.__pressed) {
            return;
        }

        root.__pressed = false;
        keyItem.pressedVisual = false;
        pressAndHoldTimer.stop();
        root.stopRepeat();
        root.cancelFinished();
    }

    touchPoints: [
        TouchPoint {
            id: touchPoint
        }
    ]

    Timer {
        id: repeatDelayTimer
        interval: root.repeatDelay
        repeat: false
        onTriggered: repeatTimer.start()
    }

    Timer {
        id: repeatTimer
        interval: root.repeatInterval
        repeat: true
        onTriggered: root.repeatTriggered()
    }

    Timer {
        id: pressAndHoldTimer
        interval: root.pressAndHoldInterval
        repeat: false
        onTriggered: root.pressAndHold(root.__lastPoint)
    }

    onPressed: {
        root.startPress(touchPoint);
    }

    onUpdated: {
        root.updatePress(touchPoint);
    }

    onReleased: {
        root.finishPress(touchPoint);
    }

    onCanceled: {
        root.cancelPress();
    }
}
