// SPDX-FileCopyrightText: 2016 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Styles
import QtQuick.Controls as QQC2
import QtQuick.Window
import QtQuick.Effects

import org.kde.kirigami as Kirigami

import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.lib as PlasmaKeyboard

KeyboardStyle {
    id: currentStyle
    readonly property bool compactSelectionList: [InputEngine.InputMode.Pinyin, InputEngine.InputMode.Cangjie, InputEngine.InputMode.Zhuyin].indexOf(InputContext.inputEngine.inputMode) !== -1

    property Binding scaleHintBinding: Binding {
        target: PlasmaKeyboard.BreezeConstants
        property: 'scaleHint'
        value: currentStyle.scaleHint
    }

    property var theme: PlasmaKeyboard.BreezeConstants

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Window

    readonly property string inputLocale: InputContext.locale

    property real inputLocaleIndicatorOpacity: 1.0
    property Timer inputLocaleIndicatorHighlightTimer: Timer {
        interval: 1000
        onTriggered: inputLocaleIndicatorOpacity = 0.5
    }
    onInputLocaleChanged: {
        inputLocaleIndicatorOpacity = 1.0
        inputLocaleIndicatorHighlightTimer.restart()
    }

    property Component component_settingsIcon: Component {
        Kirigami.Icon {
            implicitWidth: 80 * theme.keyIconScale
            implicitHeight: 80 * theme.keyIconScale
            source: "settings-configure"
        }
    }

    // Always have the keyboard panel be 30% of the screen height, or 150px (whichever is larger)
    readonly property real targetKeyboardHeight: Math.max(Screen.height * 0.3, 150)

    // The value to multiply the height by to get the width
    readonly property real aspectRatio: {
        // Ratio to just fill the screen width
        const fillScreenWidth = Screen.width / targetKeyboardHeight;
        if (PlasmaKeyboardSettings.panelFillScreenWidth) {
            return fillScreenWidth;
        }

        const targetAspectRatio = 3.0; // Target width = 3 * height
        return Math.min(fillScreenWidth, targetAspectRatio);
    }

    // Calculate width based on the height so that the keyboard height is always targetKeyboardHeight
    keyboardDesignWidth: aspectRatio * keyboardDesignHeight;
    keyboardDesignHeight: {
        if (Screen.width < 500) {
            // Phone mode
            return 800;
        } else if (Screen.width < 1200) {
            // Wider
            return 600;
        }
        // Widest
        return 700;
    }

    // The width should never be > 6 times height
    readonly property real maxWidthToHeightRatio: 6

    keyboardRelativeLeftMargin: {
        if (keyboardDesignWidth > keyboardDesignHeight * maxWidthToHeightRatio) {
            // Cap keyboard width if it's too wide
            const extraWidth = keyboardDesignWidth - (keyboardDesignHeight * maxWidthToHeightRatio);
            return (extraWidth / 2) / keyboardDesignWidth;
        }
        return 6 / keyboardDesignWidth;
    }
    keyboardRelativeRightMargin: keyboardRelativeLeftMargin
    keyboardRelativeTopMargin: 6 / keyboardDesignHeight
    keyboardRelativeBottomMargin: 6 / keyboardDesignHeight

    keyboardBackground: Rectangle {
        color: theme.keyboardBackgroundColor
    }

    keyPanel: PlasmaKeyboard.BreezeKeyPanel {
        id: keyPanel

        Item {
            id: keyContent

            QQC2.Label {
                id: keySmallText
                text: control.smallText
                visible: control.smallTextVisible
                color: theme.keySmallTextColor
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: theme.keyContentMargin / 3
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 30 * scaleHint
                    capitalization: control.uppercased ? Font.AllUppercase : Font.MixedCase
                }
            }
            Loader {
                id: loader_settingsIcon
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: theme.keyContentMargin / 3
            }
            QQC2.Label {
                id: keyText
                text: control.displayText
                color: theme.keyTextColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: control.displayText.length > 1 ? Text.AlignVCenter : Text.AlignBottom
                anchors.centerIn: parent
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 60 * scaleHint
                    capitalization: control.uppercased ? Font.AllUppercase : Font.MixedCase
                }
            }
            states: [
                State {
                    when: control.smallText === "\u2699" && control.smallTextVisible
                    PropertyChanges {
                        target: keySmallText
                        visible: false
                    }
                    PropertyChanges {
                        target: loader_settingsIcon
                        sourceComponent: component_settingsIcon
                    }
                }
            ]
        }
        states: [
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: keyContent
                    opacity: 0.75
                }
                PropertyChanges {
                    target: keyText
                    opacity: 0.05
                }
            }
        ]
    }

    backspaceKeyPanel: PlasmaKeyboard.BreezeKeyPanel {
        id: backspaceKeyPanel

        Item {
            Kirigami.Icon {
                id: backspaceKeyIcon
                anchors.centerIn: parent
                implicitHeight: 88 * theme.keyIconScale
                implicitWidth: implicitHeight
                source: "edit-clear-symbolic"
            }
        }

        states: [
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: backspaceKeyPanel.background
                    opacity: 0.8
                }
                PropertyChanges {
                    target: backspaceKeyIcon
                    opacity: 0.2
                }
            }
        ]
    }

    languageKeyPanel: PlasmaKeyboard.BreezeKeyPanel {
        id: languageKeyPanel

        Item {
            Kirigami.Icon {
                id: languageKeyIcon
                anchors.centerIn: parent
                implicitHeight: 96 * theme.keyIconScale
                source: "globe"
            }
        }

        states: [
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: languageKeyPanel.background
                    opacity: 0.8
                }
                PropertyChanges {
                    target: languageKeyIcon
                    opacity: 0.2
                }
            }
        ]
    }

    enterKeyPanel: PlasmaKeyboard.BreezeKeyPanel {
        id: enterKeyPanel

        Item {
            id: enterKeyBackground
            Kirigami.Icon {
                id: enterKeyIcon
                visible: enterKeyText.text.length === 0
                anchors.centerIn: parent
                readonly property size enterKeyIconSize: {
                    switch (control.actionId) {
                    case EnterKeyAction.Go:
                    case EnterKeyAction.Send:
                    case EnterKeyAction.Next:
                    case EnterKeyAction.Done:
                        return Qt.size(170, 119)
                    case EnterKeyAction.Search:
                        return Qt.size(148, 148)
                    default:
                        return Qt.size(211, 80)
                    }
                }
                implicitHeight: enterKeyIconSize.height * theme.keyIconScale
                source: {
                    switch (control.actionId) {
                    case EnterKeyAction.Go:
                    case EnterKeyAction.Send:
                    case EnterKeyAction.Next:
                    case EnterKeyAction.Done:
                        return "checkmark"
                    case EnterKeyAction.Search:
                        return "search-symbolic"
                    default:
                        return "keyboard-enter-symbolic"
                    }
                }
            }
            QQC2.Label {
                id: enterKeyText
                visible: text.length !== 0
                text: control.actionId !== EnterKeyAction.None ? control.displayText : ""
                clip: true
                fontSizeMode: Text.HorizontalFit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: theme.keyTextColor
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 50 * scaleHint
                    capitalization: Font.AllUppercase
                }
                anchors.fill: parent
                anchors.margins: Math.round(42 * scaleHint)
            }
        }

        states: [
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: enterKeyPanel.background
                    opacity: 0.8
                }
                PropertyChanges {
                    target: enterKeyIcon
                    opacity: 0.2
                }
                PropertyChanges {
                    target: enterKeyText
                    opacity: 0.2
                }
            }
        ]
    }

    hideKeyPanel: PlasmaKeyboard.BreezeKeyPanel {
        id: hideKeyPanel

        Item {
            Kirigami.Icon {
                id: hideKeyIcon
                anchors.centerIn: parent
                implicitHeight: 96 * theme.keyIconScale
                source: "input-keyboard-virtual-hide-symbolic"
            }
        }

        states: [
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: hideKeyPanel.background
                    opacity: 0.8
                }
                PropertyChanges {
                    target: hideKeyIcon
                    opacity: 0.2
                }
            }
        ]
    }

    shiftKeyPanel: PlasmaKeyboard.BreezeKeyPanel {
        id: shiftKeyPanel

        Item {
            Kirigami.Icon {
                id: shiftKeyIcon
                anchors.centerIn: parent
                implicitHeight: 134 * theme.keyIconScale
                source: {
                    if (InputContext.capsLockActive) {
                        return "keyboard-caps-locked-symbolic";
                    } else if (InputContext.shiftActive) {
                        return "keyboard-caps-enabled-symbolic";
                    }
                    return "keyboard-caps-disabled-symbolic";
                }
            }
        }

        states: [
            State {
                name: "capsLockActive"
                when: InputContext.capsLockActive
                PropertyChanges {
                    target: shiftKeyPanel
                    color: theme.capsLockKeyAccentColor
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: shiftKeyPanel.background
                    opacity: 0.8
                }
                PropertyChanges {
                    target: shiftKeyIcon
                    opacity: 0.2
                }
            }
        ]
    }

    spaceKeyPanel: PlasmaKeyboard.BreezeKeyPanel {
        id: spaceKeyPanel

        Item {
            QQC2.Label {
                id: spaceKeyText
                anchors.centerIn: parent
                text: Qt.locale(InputContext.locale).nativeLanguageName
                color: theme.keyTextColor
                opacity: inputLocaleIndicatorOpacity
                Behavior on opacity { PropertyAnimation { duration: 250 } }
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 35 * scaleHint
                }
            }
        }

        states: [
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: spaceKeyBackground
                    opacity: 0.8
                }
            }
        ]
    }

    symbolKeyPanel: PlasmaKeyboard.BreezeKeyPanel {
        id: symbolKeyPanel

        Item {
            QQC2.Label {
                id: symbolKeyText
                anchors.centerIn: parent
                text: control.displayText
                color: theme.keyTextColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 40 * scaleHint
                    capitalization: Font.AllUppercase
                }
            }
        }

        states: [
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: symbolKeyPanel.background
                    opacity: 0.8
                }
                PropertyChanges {
                    target: symbolKeyText
                    opacity: 0.2
                }
            }
        ]
    }

    modeKeyPanel: PlasmaKeyboard.BreezeKeyPanel {
        id: modeKeyPanel

        Item {
            id: modeKeyBackground
            QQC2.Label {
                id: modeKeyText
                text: control.displayText
                color: theme.keyTextColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent
                anchors.margins: theme.keyContentMargin
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 40 * scaleHint
                    capitalization: Font.AllUppercase
                }
            }
            Rectangle {
                id: modeKeyIndicator
                implicitHeight: parent.height * 0.1
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.leftMargin: parent.width * 0.4
                anchors.rightMargin: parent.width * 0.4
                anchors.bottomMargin: parent.height * 0.12
                color: theme.modeKeyAccentColor
                radius: theme.buttonRadius
                visible: control.mode
            }
        }

        states: [
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: modeKeyPanel.background
                    opacity: 0.8
                }
                PropertyChanges {
                    target: modeKeyText
                    opacity: 0.2
                }
            }
        ]
    }

    handwritingKeyPanel: PlasmaKeyboard.BreezeKeyPanel {
        id: handwritingKeyPanel

        Item {
            Kirigami.Icon {
                id: hwrKeyIcon
                anchors.centerIn: parent
                implicitHeight: 127 * theme.keyIconScale
                source: (keyboard.handwritingMode ? "edit-select-text-symbolic" : "draw-freehand-symbolic")
            }
        }

        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: handwritingKeyPanel.background
                    opacity: 0.80
                }
                PropertyChanges {
                    target: hwrKeyIcon
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: handwritingKeyPanel.background
                    opacity: 0.8
                }
                PropertyChanges {
                    target: hwrKeyIcon
                    opacity: 0.2
                }
            }
        ]
    }

    characterPreviewMargin: 0
    characterPreviewDelegate: Item {
        property string text
        property string flickLeft
        property string flickTop
        property string flickRight
        property string flickBottom
        readonly property bool flickKeysSet: flickLeft || flickTop || flickRight || flickBottom
        readonly property bool flickKeysVisible: text && flickKeysSet &&
                                                 text !== flickLeft && text !== flickTop && text !== flickRight && text !== flickBottom
        id: characterPreview
        PlasmaKeyboard.BreezePopup {
            id: characterPreviewBackground
            theme: currentStyle.theme
            anchors.fill: parent
            readonly property int largeTextHeight: Math.round(height / 3 * 2)
            readonly property int smallTextHeight: Math.round(height / 3)
            readonly property int smallTextMargin: Math.round(3 * scaleHint)

            QQC2.Label {
                id: characterPreviewText
                color: theme.popupTextColor
                text: characterPreview.text
                fontSizeMode: Text.VerticalFit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                height: characterPreviewBackground.largeTextHeight
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 82 * scaleHint
                }
            }
            QQC2.Label {
                color: theme.popupTextColor
                text: characterPreview.flickLeft
                visible: characterPreview.flickKeysVisible
                opacity: 0.8
                fontSizeMode: Text.VerticalFit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.left: parent.left
                anchors.leftMargin: characterPreviewBackground.smallTextMargin
                anchors.verticalCenter: parent.verticalCenter
                height: characterPreviewBackground.smallTextHeight
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 62 * scaleHint
                }
            }
            QQC2.Label {
                color: theme.popupTextColor
                text: characterPreview.flickTop
                visible: characterPreview.flickKeysVisible
                opacity: 0.8
                fontSizeMode: Text.VerticalFit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.top: parent.top
                anchors.topMargin: characterPreviewBackground.smallTextMargin
                anchors.horizontalCenter: parent.horizontalCenter
                height: characterPreviewBackground.smallTextHeight
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 62 * scaleHint
                }
            }
            QQC2.Label {
                color: theme.popupTextColor
                text: characterPreview.flickRight
                visible: characterPreview.flickKeysVisible
                opacity: 0.8
                fontSizeMode: Text.VerticalFit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.right: parent.right
                anchors.rightMargin: characterPreviewBackground.smallTextMargin
                anchors.verticalCenter: parent.verticalCenter
                height: characterPreviewBackground.smallTextHeight
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 62 * scaleHint
                }
            }
            QQC2.Label {
                color: theme.popupTextColor
                text: characterPreview.flickBottom
                visible: characterPreview.flickKeysVisible
                opacity: 0.8
                fontSizeMode: Text.VerticalFit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: characterPreviewBackground.smallTextMargin
                anchors.horizontalCenter: parent.horizontalCenter
                height: characterPreviewBackground.smallTextHeight
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 62 * scaleHint
                }
            }
            states: State {
                name: "flickKeysVisible"
                when: characterPreview.flickKeysVisible
                PropertyChanges {
                    target: characterPreviewText
                    height: characterPreviewBackground.smallTextHeight
                }
            }
        }
    }

    alternateKeysListItemWidth: 120 * scaleHint
    alternateKeysListItemHeight: 170 * scaleHint
    alternateKeysListDelegate: Item {
        id: alternateKeysListItem
        width: alternateKeysListItemWidth
        height: alternateKeysListItemHeight
        QQC2.Label {
            id: listItemText
            text: model.text
            color: alternateKeysListItem.ListView.isCurrentItem ? theme.popupTextSelectedColor : theme.popupTextColor
            opacity: 0.8
            font {
                family: theme.fontFamily
                weight: Font.Light
                pixelSize: 60 * scaleHint
            }
            anchors.centerIn: parent
        }
        states: State {
            name: "current"
            when: alternateKeysListItem.ListView.isCurrentItem
            PropertyChanges {
                target: listItemText
                opacity: 1
            }
        }
    }
    alternateKeysListHighlight: Rectangle {
        color: theme.popupHighlightColor
        radius: theme.buttonRadius
        border.color: theme.popupHighlightBorderColor
        border.width: 1
    }
    alternateKeysListBackground: Item {
        PlasmaKeyboard.BreezePopup {
            theme: currentStyle.theme
            readonly property real margin: 20 * scaleHint
            x: -margin
            y: -margin
            width: parent.width + 2 * margin
            height: parent.height + 2 * margin
        }
    }

    selectionListHeight: 85 * scaleHint
    selectionListDelegate: SelectionListItem {
        id: selectionListItem
        width: Math.round(selectionListLabel.width + selectionListLabel.anchors.leftMargin * 2)
        QQC2.Label {
            id: selectionListLabel
            anchors.left: parent.left
            anchors.leftMargin: Math.round((compactSelectionList ? 50 : 140) * scaleHint)
            anchors.verticalCenter: parent.verticalCenter
            text: decorateText(display, wordCompletionLength)
            color: theme.selectionListTextColor
            opacity: 0.9
            font {
                family: theme.fontFamily
                weight: Font.Light
                pixelSize: 44 * scaleHint
            }
            function decorateText(text, wordCompletionLength) {
                if (wordCompletionLength > 0) {
                    return text.slice(0, -wordCompletionLength) + '<u>' + text.slice(-wordCompletionLength) + '</u>'
                }
                return text
            }
        }
        Rectangle {
            id: selectionListSeparator
            width: 4 * scaleHint
            height: 36 * scaleHint
            radius: 2
            color: theme.selectionListSeparatorColor
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.left
        }
        states: State {
            name: "current"
            when: selectionListItem.ListView.isCurrentItem
            PropertyChanges {
                target: selectionListLabel
                opacity: 1
            }
        }
    }
    selectionListBackground: Rectangle {
        color: theme.selectionListBackgroundColor
    }
    selectionListAdd: Transition {
        NumberAnimation { property: "y"; from: wordCandidateView.height; duration: 200 }
        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 200 }
    }
    selectionListRemove: Transition {
        NumberAnimation { property: "y"; to: -wordCandidateView.height; duration: 200 }
        NumberAnimation { property: "opacity"; to: 0; duration: 200 }
    }

    navigationHighlight: Rectangle {
        color: theme.navigationHighlightColor
        border.color: theme.navigationHighlightBorderColor
        border.width: 3
        radius: theme.buttonRadius + border.width // Adjust due to border width
    }

    traceInputKeyPanelDelegate: TraceInputKeyPanel {
        id: traceInputKeyPanel
        traceMargins: theme.keyBackgroundMargin
        Rectangle {
            id: traceInputKeyPanelBackground
            radius: theme.buttonRadius
            color: theme.normalKeyBackgroundColor
            anchors.fill: traceInputKeyPanel
            anchors.margins: theme.keyBackgroundMargin
            QQC2.Label {
                id: hwrInputModeIndicator
                visible: control.patternRecognitionMode === InputEngine.PatternRecognitionMode.Handwriting
                text: {
                    switch (InputContext.inputEngine.inputMode) {
                    case InputEngine.InputMode.Numeric:
                        if (["ar", "fa"].indexOf(InputContext.locale.substring(0, 2)) !== -1)
                            return "\u0660\u0661\u0662"
                        // Fallthrough
                    case InputEngine.InputMode.Dialable:
                        return "123"
                    case InputEngine.InputMode.Greek:
                        return "ΑΒΓ"
                    case InputEngine.InputMode.Cyrillic:
                        return "АБВ"
                    case InputEngine.InputMode.Arabic:
                        if (InputContext.locale.substring(0, 2) === "fa")
                            return "\u0627\u200C\u0628\u200C\u067E"
                        return "\u0623\u200C\u0628\u200C\u062C"
                    case InputEngine.InputMode.Hebrew:
                        return "\u05D0\u05D1\u05D2"
                    case InputEngine.InputMode.ChineseHandwriting:
                        return "中文"
                    case InputEngine.InputMode.JapaneseHandwriting:
                        return "日本語"
                    case InputEngine.InputMode.KoreanHandwriting:
                        return "한국어"
                    case InputEngine.InputMode.Thai:
                        return "กขค"
                    default:
                        return "Abc"
                    }
                }
                color: theme.keyTextColor
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: theme.keyContentMargin
                font {
                    family: theme.fontFamily
                    weight: Font.Light
                    pixelSize: 44 * scaleHint
                    capitalization: {
                        if (InputContext.capsLockActive)
                            return Font.AllUppercase
                        if (InputContext.shiftActive)
                            return Font.MixedCase
                        return Font.AllLowercase
                    }
                }
            }
        }
        Canvas {
            id: traceInputKeyGuideLines
            anchors.fill: traceInputKeyPanelBackground
            opacity: 0.1
            onPaint: {
                var ctx = getContext("2d")
                ctx.lineWidth = 1
                ctx.strokeStyle = Qt.rgba(0xFF, 0xFF, 0xFF)
                ctx.clearRect(0, 0, width, height)
                var i
                var margin = Math.round(30 * scaleHint)
                if (control.horizontalRulers) {
                    for (i = 0; i < control.horizontalRulers.length; i++) {
                        ctx.beginPath()
                        var y = Math.round(control.horizontalRulers[i])
                        var rightMargin = Math.round(width - margin)
                        if (i + 1 === control.horizontalRulers.length) {
                            ctx.moveTo(margin, y)
                            ctx.lineTo(rightMargin, y)
                        } else {
                            var dashLen = Math.round(20 * scaleHint)
                            for (var dash = margin, dashCount = 0;
                                 dash < rightMargin; dash += dashLen, dashCount++) {
                                if ((dashCount & 1) === 0) {
                                    ctx.moveTo(dash, y)
                                    ctx.lineTo(Math.min(dash + dashLen, rightMargin), y)
                                }
                            }
                        }
                        ctx.stroke()
                    }
                }
                if (control.verticalRulers) {
                    for (i = 0; i < control.verticalRulers.length; i++) {
                        ctx.beginPath()
                        ctx.moveTo(control.verticalRulers[i], margin)
                        ctx.lineTo(control.verticalRulers[i], Math.round(height - margin))
                        ctx.stroke()
                    }
                }
            }
            Connections {
                target: control
                function onHorizontalRulersChanged() { traceInputKeyGuideLines.requestPaint() }
                function onVerticalRulersChanged() { traceInputKeyGuideLines.requestPaint() }
            }
        }
    }

    traceCanvasDelegate: TraceCanvas {
        id: traceCanvas
        onAvailableChanged: {
            if (!available)
                return
            var ctx = getContext("2d")
            if (parent.canvasType === "fullscreen") {
                ctx.lineWidth = 10
                ctx.strokeStyle = Qt.rgba(0, 0, 0)
            } else {
                ctx.lineWidth = 10 * scaleHint
                ctx.strokeStyle = Qt.rgba(0xFF, 0xFF, 0xFF)
            }
            ctx.lineCap = "round"
            ctx.fillStyle = ctx.strokeStyle
        }
        autoDestroyDelay: 800
        onTraceChanged: if (trace === null) opacity = 0
        Behavior on opacity { PropertyAnimation { easing.type: Easing.OutCubic; duration: 150 } }
    }

    popupListDelegate: SelectionListItem {
        property real cursorAnchor: popupListLabel.x + popupListLabel.width
        id: popupListItem
        width: popupListLabel.width + popupListLabel.anchors.leftMargin * 2
        height: popupListLabel.height + popupListLabel.anchors.topMargin * 2
        QQC2.Label {
            id: popupListLabel
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: popupListLabel.height / 2
            anchors.topMargin: popupListLabel.height / 3
            text: decorateText(display, wordCompletionLength)
            color: theme.popupTextColor
            opacity: 0.8
            font {
                family: theme.fontFamily
                weight: Font.Light
                pixelSize: Qt.inputMethod.cursorRectangle.height * 0.8
            }
            function decorateText(text, wordCompletionLength) {
                if (wordCompletionLength > 0) {
                    return text.slice(0, -wordCompletionLength) + '<u>' + text.slice(-wordCompletionLength) + '</u>'
                }
                return text
            }
        }
        states: State {
            name: "current"
            when: popupListItem.ListView.isCurrentItem
            PropertyChanges {
                target: popupListLabel
                opacity: 1.0
            }
        }
    }

    popupListBackground: PlasmaKeyboard.BreezePopup {
        theme: currentStyle.theme
    }

    popupListAdd: Transition {}

    popupListRemove: Transition {}

    languagePopupListEnabled: true

    languageListDelegate: SelectionListItem {
        id: languageListItem
        width: languageNameTextMetrics.width * 17
        height: languageNameTextMetrics.height + languageListLabel.anchors.topMargin + languageListLabel.anchors.bottomMargin
        QQC2.Label {
            id: languageListLabel
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: languageNameTextMetrics.height / 2
            anchors.rightMargin: anchors.leftMargin
            anchors.topMargin: languageNameTextMetrics.height / 3
            anchors.bottomMargin: anchors.topMargin
            text: languageNameFormatter.elidedText
            color: theme.popupTextColor
            opacity: 0.8
            font {
                family: theme.fontFamily
                weight: Font.Light
                pixelSize: 44 * scaleHint
            }
        }
        TextMetrics {
            id: languageNameTextMetrics
            font {
                family: theme.fontFamily
                weight: Font.Light
                pixelSize: 44 * scaleHint
            }
            text: "X"
        }
        TextMetrics {
            id: languageNameFormatter
            font {
                family: theme.fontFamily
                weight: Font.Light
                pixelSize: 44 * scaleHint
            }
            elide: Text.ElideRight
            elideWidth: languageListItem.width - languageListLabel.anchors.leftMargin - languageListLabel.anchors.rightMargin
            text: displayName
        }
        states: State {
            name: "current"
            when: languageListItem.ListView.isCurrentItem
            PropertyChanges {
                target: languageListLabel
                opacity: 1
            }
        }
    }

    languageListHighlight: Rectangle {
        color: theme.popupHighlightColor
        radius: theme.buttonRadius
        border.color: theme.popupHighlightBorderColor
        border.width: 1
    }

    languageListBackground: Item {
        PlasmaKeyboard.BreezePopup {
            theme: currentStyle.theme
            readonly property real backgroundMargin: 20 * scaleHint
            x: -backgroundMargin
            y: -backgroundMargin
            width: parent.width + 2 * backgroundMargin
            height: parent.height + 2 * backgroundMargin
        }
    }

    languageListAdd: Transition {
        // NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 200 }
    }

    languageListRemove: Transition {
        // NumberAnimation { property: "opacity"; to: 0; duration: 200 }
    }

    selectionHandle: Kirigami.Icon {
        implicitWidth: 20
        source: "selection-end-symbolic" // TODO: better icon?
    }

    fullScreenInputContainerBackground: Rectangle {
        color: "#FFF"
    }

    fullScreenInputBackground: Rectangle {
        color: "#FFF"
    }

    fullScreenInputMargins: Math.round(15 * scaleHint)

    fullScreenInputPadding: Math.round(30 * scaleHint)

    fullScreenInputCursor: Rectangle {
        width: 1
        color: "#000"
        visible: parent.blinkStatus
    }

    fullScreenInputFont.pixelSize: 58 * scaleHint

    functionPopupListDelegate: Item {
        id: functionPopupListItem
        readonly property real iconMargin: 40 * scaleHint
        readonly property real iconWidth: 96 * theme.keyIconScale
        readonly property real iconHeight: 96 * theme.keyIconScale
        width: iconWidth + 2 * iconMargin
        height: iconHeight + 2 * iconMargin
        Kirigami.Icon {
            id: functionIcon
            anchors.centerIn: parent
            implicitHeight: iconHeight
            source: {
                switch (keyboardFunction) {
                case QtVirtualKeyboard.KeyboardFunction.HideInputPanel:
                    return "input-keyboard-virtual-hide-symbolic"
                case QtVirtualKeyboard.KeyboardFunction.ChangeLanguage:
                    return "globe-symbolic"
                case QtVirtualKeyboard.KeyboardFunction.ToggleHandwritingMode:
                    return (keyboard.handwritingMode ? "edit-select-text-symbolic" : "draw-freehand-symbolic") // TODO: better icons?
                }
            }
        }
    }

    functionPopupListBackground: Item {
        PlasmaKeyboard.BreezePopup {
            theme: currentStyle.theme
            readonly property real backgroundMargin: 20 * scaleHint
            x: -backgroundMargin
            y: -backgroundMargin
            width: parent.width + 2 * backgroundMargin
            height: parent.height + 2 * backgroundMargin
        }
    }

    functionPopupListHighlight: Rectangle {
        color: theme.popupHighlightColor
        radius: theme.buttonRadius
    }
}
