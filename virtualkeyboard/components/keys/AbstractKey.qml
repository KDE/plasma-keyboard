// SPDX-FileCopyrightText: 2025-2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard

// Base key on the keyboard.

Item {
    id: root

    signal clicked()

    /**
     * Text sent by the key and used as the default label.
     */
    property string text: ""

    /**
     * Icon name shown instead of text.
     */
    property string iconName: ""

    /**
     * Relative width hint used by the layout row for horizontal space.
     */
    property real weight: parent && parent.defaultKeyWeight !== undefined ? parent.defaultKeyWeight : 100

    /**
     * Text shown on the key face.
     * This can differ from what text is actually committed (see `text`).
     */
    property string displayText: text

    /**
     * Small label shown in the corner of the key.
     */
    property string smallText: ""

    /**
     * Alternate characters offered from the long-press popup.
     * Layouts can provide this either as a string or as an explicit list.
     */
    property var alternativeKeys: []
    readonly property var _normalizedAlternativeKeys: typeof alternativeKeys === "string" ? alternativeKeys.split("") : (alternativeKeys || [])

    /**
     * Alternate keys after filtering out text itself.
     */
    readonly property var effectiveAlternativeKeys: {
        const textIndex = _normalizedAlternativeKeys.indexOf(text)
        if (textIndex === -1) {
            return _normalizedAlternativeKeys
        }
        return _normalizedAlternativeKeys.slice(0, textIndex).concat(_normalizedAlternativeKeys.slice(textIndex + 1))
    }
    readonly property int effectiveAlternativeKeysHighlightIndex: {
        const index = _normalizedAlternativeKeys.indexOf(text)
        return index > 0 && (index + 1) === _normalizedAlternativeKeys.length ? index - 1 : index
    }

    /**
     * Display-only override for labels shown in the alternate-key popup.
     */
    property var displayAlternativeKeys: effectiveAlternativeKeys

    /**
     * Whether the button "looks" pressed.
     */
    property bool pressedVisual: false

    /**
     * Whether to ignore modifiers (ex. shift and capslock).
     */
    property bool noModifier: false

    /**
     * Enables repeating activation while the key is held.
     */
    property bool repeat: false

    /**
     * Uses the secondary key background styling (TODO: currently no visual difference).
     */
    property bool secondaryStyle: false

    /**
     * Requests the selected key visual treatment.
     */
    property bool highlighted: false

    /**
     * Marks this key as a function key for styling and preview behavior.
     */
    property bool functionKey: false

    /**
     * Controls whether the key-preview popup should be shown while pressed.
     */
    property bool showPreview: enabled && !functionKey

    /**
     * Whether the key label should follow uppercase display rules.
     */
    property bool uppercased: VirtualKeyboard.inputEngine && VirtualKeyboard.inputEngine.uppercase && !noModifier

    /**
     * Pixel size used for the main key label.
     */
    property real textPixelSize: ((displayText.length > 1) ? 40 : 60) * scaleHint

    /**
     * Local scale factor used for key text sizing.
     */
    property real scaleHint: Math.max(0.3, Math.min(width / 220, height / 140))

    /**
     * Size used for icon-based keys.
     */
    readonly property real iconSize: Math.round((functionKey ? 88 : 80) * BreezeConstants.keyIconScale)

    implicitHeight: Math.round(72 * BreezeConstants.scaleHint)

    // HACK: Layout.preferredWidth is used as a sort of size hint, not an exact size.
    // If the weight is too large, preferredWidth gets capped out and stuff starts breaking
    Layout.preferredWidth: weight / Math.max(parent && parent.defaultKeyWeight !== undefined ? parent.defaultKeyWeight : 100, 1)
    Layout.minimumWidth: 0
    Layout.fillWidth: true
    Layout.fillHeight: true

    function trigger() {
        if (!enabled) {
            return;
        }
        clicked();
    }

    function playPressFeedback() {
        Feedback.play(Feedback.Press);
    }

    Item {
        anchors.fill: parent

        Item {
            anchors.fill: parent
            anchors.margins: BreezeConstants.keyBackgroundMargin

            Kirigami.ShadowedRectangle {
                anchors.fill: parent
                color: {
                    if (root.pressedVisual) {
                        return root.secondaryStyle ? BreezeConstants.secondaryKeyPressedBackgroundColor : BreezeConstants.normalKeyPressedBackgroundColor
                    }
                    if (root.highlighted) {
                        return BreezeConstants.highlightedKeyBackgroundColor
                    }
                    return root.secondaryStyle ? BreezeConstants.secondaryKeyBackgroundColor : BreezeConstants.normalKeyBackgroundColor
                }
                radius: BreezeConstants.buttonRadius

                shadow.color: Qt.rgba(0, 0, 0, 0.2)
                shadow.size: 3
                shadow.yOffset: 1
            }

            QQC2.Label {
                id: mainTextLabel
                anchors.centerIn: parent
                text: root.uppercased ? root.displayText.toUpperCase() : root.displayText
                visible: text.length > 0 && root.iconName.length === 0
                color: BreezeConstants.keyTextColor

                font {
                    family: BreezeConstants.fontFamily
                    weight: Font.Light
                    pixelSize: root.textPixelSize
                }
            }

            Kirigami.Icon {
                anchors.centerIn: parent
                source: root.iconName
                visible: root.iconName.length > 0
                implicitHeight: root.iconSize
                implicitWidth: root.iconSize
                height: implicitHeight
                width: implicitWidth
            }

            QQC2.Label {
                id: smallTextLabel
                text: root.smallText
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: BreezeConstants.keyContentMargin / 3
                color: BreezeConstants.keySmallTextColor

                font {
                    family: BreezeConstants.fontFamily
                    weight: Font.Light
                    pixelSize: 25 * scaleHint
                }

                visible: text.length > 0
            }
        }
    }
}
