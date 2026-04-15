// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick

import org.kde.plasma.keyboard.virtualkeyboard

// Key for 12-key layouts that supports directional flicks and repeated taps.

AbstractKey {
    id: root

    enum Direction {
        Center,
        Left,
        Top,
        Right,
        Bottom
    }

    /**
     * Primary label shown on the key when no directional flick is active.
     */
    property string baseText: text

    /**
     * Text sent to the input method when the key is tapped without flicking.
     */
    property string baseCommitText: baseText

    /**
     * Commit texts for the directional alternatives.
     * The order matches the visual alternative order after removing the base
     * text from alternativeKeys.
     */
    property var alternativeCommitTexts: []

    /**
     * Enables repeated-tap cycling for 12-key style layouts.
     * When enabled, repeated center taps ask the input method to replace the
     * previously entered chunk with the next entry from the tap sequence.
     */
    property bool tapSequenceEnabled: false

    /**
     * Commit texts used for repeated center taps. Leave empty to use the
     * base commit text followed by the directional alternative commit texts.
     */
    property var tapSequenceCommitTexts: []

    /**
     * Currently highlighted label while the pointer is down.
     */
    property string currentText: baseText

    /**
     * Commit text paired with currentText.
     */
    property string currentCommitText: baseCommitText

    property int __tapSequenceIndex: -1
    property point __pressPoint: Qt.point(0, 0)

    /**
     * Distance from the press point that still counts as a center tap.
     */
    readonly property real __centerRadius: width * 0.4

    /**
     * The list of flickable alternate keys, not including the base character.
     */
    readonly property var flickKeys: {
        // Alternative keys can either be string or array
        const keys = typeof alternativeKeys === "string" ? alternativeKeys.split("") : (alternativeKeys || []);
        const baseIndex = keys.indexOf(baseText);
        if (baseIndex === -1) {
            return keys;
        }
        return keys.slice(0, baseIndex).concat(keys.slice(baseIndex + 1));
    }

    // The visual alternative order is center, left, top, right, bottom.
    readonly property string flickLeft: flickKeys.length > 0 ? flickKeys[0] : ""
    readonly property string flickTop: flickKeys.length > 2 ? flickKeys[1] : ""
    readonly property string flickBottom: flickKeys.length > 3 ? flickKeys[3] : (flickKeys.length > 2 ? flickKeys[2] : "")
    readonly property string flickRight: flickKeys.length > 3 ? flickKeys[2] : (flickKeys.length === 2 ? flickKeys[1] : "")
    readonly property string __flickCommitLeft: alternativeCommitTexts.length > 0 ? alternativeCommitTexts[0] : flickLeft
    readonly property string __flickCommitTop: alternativeCommitTexts.length > 2 ? alternativeCommitTexts[1] : flickTop
    readonly property string __flickCommitBottom: alternativeCommitTexts.length > 3 ? alternativeCommitTexts[3] : (alternativeCommitTexts.length > 2 ? alternativeCommitTexts[2] : flickBottom)
    readonly property string __flickCommitRight: alternativeCommitTexts.length > 3 ? alternativeCommitTexts[2] : (alternativeCommitTexts.length === 2 ? alternativeCommitTexts[1] : flickRight)

    /**
     * Ordered commit texts used for repeated center taps.
     */
    readonly property var __tapSequenceCommitTexts: tapSequenceCommitTexts.length > 0 ? tapSequenceCommitTexts : [baseCommitText].concat(alternativeCommitTexts)

    displayText: pressedVisual ? currentText : baseText

    function __clearTapSequence() {
        __tapSequenceIndex = -1;
        tapSequenceTimer.stop();
        pressedVisual = false;
        FlickKeyPrivate.clearActiveTapSequenceKey(root);
    }

    function __resetCurrentText() {
        currentText = baseText;
        currentCommitText = baseCommitText;
    }

    // Get the angle that the given point is from the initial "tap point"
    function __angle(point) {
        const dx = point.x - __pressPoint.x;
        const dy = point.y - __pressPoint.y;
        const theta = Math.atan2(-dy, dx) * 360 / (2 * Math.PI);
        return theta < 0 ? theta + 360 : theta;
    }

    function __distance(point) {
        const dx = point.x - __pressPoint.x;
        const dy = point.y - __pressPoint.y;
        return Math.sqrt(dx * dx + dy * dy);
    }

    function __directionForPoint(point) {
        if (__distance(point) < __centerRadius) {
            return FlickKey.Direction.Center;
        }

        const currentAngle = __angle(point);
        if (currentAngle < 45 || currentAngle > 315) {
            return FlickKey.Direction.Right;
        }
        if (currentAngle < 135) {
            return FlickKey.Direction.Top;
        }
        if (currentAngle < 225) {
            return FlickKey.Direction.Left;
        }
        return FlickKey.Direction.Bottom;
    }

    function __keyForText(textValue) {
        return textValue && textValue.length === 1 ? textValue.charCodeAt(0) : Qt.Key_unknown;
    }

    function __commitTapSequence() {
        if (!tapSequenceEnabled || __tapSequenceCommitTexts.length === 0) {
            return false;
        }

        let nextIndex = 0;
        if (tapSequenceTimer.running) {
            // Cycle through the possible keys if the tapSequenceTimer is running
            nextIndex = (__tapSequenceIndex + 1) % __tapSequenceCommitTexts.length;
            if (__tapSequenceIndex >= 0 && VirtualKeyboard.inputEngine.textComposer.replaceLastInput(__tapSequenceCommitTexts[nextIndex])) {
                __tapSequenceIndex = nextIndex;
                tapSequenceTimer.restart();
                pressedVisual = true;
                FlickKeyPrivate.setActiveTapSequenceKey(root);
                return true;
            }
        }

        __tapSequenceIndex = 0;
        tapSequenceTimer.restart();
        pressedVisual = true;
        FlickKeyPrivate.setActiveTapSequenceKey(root);
        return false;
    }

    function trigger() {
        if (!enabled) {
            return;
        }
        VirtualKeyboard.inputEngine.sendTextComposerKey(root.__keyForText(root.baseText), root.baseCommitText);
        clicked();
    }

    function __updateCurrentText(x, y) {
        const direction = __directionForPoint(Qt.point(x, y));
        switch (direction) {
        case FlickKey.Direction.Left:
            currentText = flickLeft || baseText;
            currentCommitText = __flickCommitLeft || currentText;
            break;
        case FlickKey.Direction.Top:
            currentText = flickTop || baseText;
            currentCommitText = __flickCommitTop || currentText;
            break;
        case FlickKey.Direction.Right:
            currentText = flickRight || baseText;
            currentCommitText = __flickCommitRight || currentText;
            break;
        case FlickKey.Direction.Bottom:
            currentText = flickBottom || baseText;
            currentCommitText = __flickCommitBottom || currentText;
            break;
        default:
            __resetCurrentText();
            break;
        }
    }

    Timer {
        id: tapSequenceTimer
        interval: 1000
        repeat: false
        onTriggered: root.__clearTapSequence();
    }

    KeyMouseArea {
        keyItem: root

        onPressStarted: (point) => {
            root.__pressPoint = Qt.point(point.x, point.y);
            root.__resetCurrentText();
            VirtualKeyboard.flickPreviewPopup.openForKey(root);
        }

        onPositionChanged: (point) => {
            root.__updateCurrentText(point.x, point.y);
            VirtualKeyboard.flickPreviewPopup.updateFromKey(root);
        }

        onReleaseFinished: {
            VirtualKeyboard.flickPreviewPopup.close();

            const usedExplicitFlick = root.currentText !== root.baseText || root.currentCommitText !== root.baseCommitText;
            if (usedExplicitFlick) {
                root.__clearTapSequence();
            } else if (root.__commitTapSequence()) {
                root.clicked();
                root.__resetCurrentText();
                return;
            }

            VirtualKeyboard.inputEngine.sendTextComposerKey(root.__keyForText(root.currentText), root.currentCommitText);
            root.clicked();
            root.__resetCurrentText();
        }

        onCancelFinished: {
            VirtualKeyboard.flickPreviewPopup.close();
            root.__clearTapSequence();
            root.__resetCurrentText();
        }
    }

    Component.onDestruction: FlickKeyPrivate.clearActiveTapSequenceKey(root)
}
