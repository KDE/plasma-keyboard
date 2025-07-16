// SPDX-FileCopyrightText: 2016 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Styles
import QtQuick.Controls as QQC2
import QtQuick.Window
import QtQuick.Effects

import org.kde.kirigami as Kirigami

import org.kde.plasma.keyboard

KeyboardStyle {
    id: currentStyle
    readonly property bool compactSelectionList: [InputEngine.InputMode.Pinyin, InputEngine.InputMode.Cangjie, InputEngine.InputMode.Zhuyin].indexOf(InputContext.inputEngine.inputMode) !== -1

    readonly property Theme theme: Theme {
        scaleHint: currentStyle.scaleHint
    }

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

    keyboardDesignWidth: {
        if (Screen.width < 500) {
            // Phone mode
            return 1300;
        } else if (Screen.width < 1200) {
            // Wider
            return 1600;
        }
        // Widest
        return 2560;
    }
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
    keyboardRelativeLeftMargin: 6 / keyboardDesignWidth
    keyboardRelativeRightMargin: 6 / keyboardDesignWidth
    keyboardRelativeTopMargin: 6 / keyboardDesignHeight
    keyboardRelativeBottomMargin: 6 / keyboardDesignHeight

    keyboardBackground: Rectangle {
        color: theme.keyboardBackgroundColor
    }

    keyPanel: BreezeKeyPanel {
        id: keyPanel
        theme: currentStyle.theme

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
                    pixelSize: 40 * scaleHint
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

    backspaceKeyPanel: BreezeKeyPanel {
        id: backspaceKeyPanel
        theme: currentStyle.theme

        Item {
            Kirigami.Icon {
                id: backspaceKeyIcon
                anchors.centerIn: parent
                implicitHeight: 88 * theme.keyIconScale
                implicitWidth: implicitHeight
                source: "edit-clear-symoblic"
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

    languageKeyPanel: BreezeKeyPanel {
        id: languageKeyPanel
        theme: currentStyle.theme

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

    enterKeyPanel: BreezeKeyPanel {
        id: enterKeyPanel
        theme: currentStyle.theme

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

    hideKeyPanel: BreezeKeyPanel {
        id: hideKeyPanel
        theme: currentStyle.theme

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

    shiftKeyPanel: BreezeKeyPanel {
        id: shiftKeyPanel
        theme: currentStyle.theme

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

    spaceKeyPanel: BreezeKeyPanel {
        id: spaceKeyPanel
        theme: currentStyle.theme

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

    symbolKeyPanel: BreezeKeyPanel {
        id: symbolKeyPanel
        theme: currentStyle.theme

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

    modeKeyPanel: BreezeKeyPanel {
        id: modeKeyPanel
        theme: currentStyle.theme

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

    handwritingKeyPanel: BreezeKeyPanel {
        id: handwritingKeyPanel
        theme: currentStyle.theme

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
        Rectangle {
            id: characterPreviewBackground
            anchors.fill: parent
            color: theme.popupBackgroundColor
            radius: theme.buttonRadius
            readonly property int largeTextHeight: Math.round(height / 3 * 2)
            readonly property int smallTextHeight: Math.round(height / 3)
            readonly property int smallTextMargin: Math.round(3 * scaleHint)

            border {
                width: 1
                color: theme.popupBorderColor
            }

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
    }
    alternateKeysListBackground: Item {
        Rectangle {
            readonly property real margin: 20 * scaleHint
            x: -margin
            y: -margin
            width: parent.width + 2 * margin
            height: parent.height + 2 * margin
            radius: theme.buttonRadius
            color: theme.popupBackgroundColor
            border {
                width: 1
                color: theme.popupBorderColor
            }
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
        color: "transparent"
        border.color: theme.navigationHighlightColor
        border.width: 5
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

    popupListBackground: Rectangle {
        color: theme.popupBackgroundColor
        border {
            width: 1
            color: theme.popupBorderColor
        }
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
    }

    languageListBackground: Rectangle {
        color: theme.popupBackgroundColor
        border {
            width: 1
            color: theme.popupBorderColor
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
        Rectangle {
            readonly property real backgroundMargin: 20 * scaleHint
            x: -backgroundMargin
            y: -backgroundMargin
            width: parent.width + 2 * backgroundMargin
            height: parent.height + 2 * backgroundMargin
            radius: theme.buttonRadius
            color: theme.popupBackgroundColor
            border {
                width: 1
                color: theme.popupBorderColor
            }
        }
    }

    functionPopupListHighlight: Rectangle {
        color: theme.popupHighlightColor
        radius: theme.buttonRadius
    }
}
