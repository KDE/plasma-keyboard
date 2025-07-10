// SPDX-FileCopyrightText: 2016 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Styles
import QtQuick.Controls as QQC2
import QtQuick.Window

import org.kde.kirigami as Kirigami

import org.kde.plasma.keyboard

KeyboardStyle {
    id: currentStyle
    readonly property bool compactSelectionList: [InputEngine.InputMode.Pinyin, InputEngine.InputMode.Cangjie, InputEngine.InputMode.Zhuyin].indexOf(InputContext.inputEngine.inputMode) !== -1
    readonly property string fontFamily: Kirigami.Theme.defaultFont.family
    readonly property real keyBackgroundMargin: Math.round(8 * scaleHint)
    readonly property real keyContentMargin: Math.round(40 * scaleHint)
    readonly property real keyIconScale: scaleHint * 0.8

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Window

    readonly property string inputLocale: InputContext.locale
    property color primaryColor: Kirigami.Theme.backgroundColor
    property color primaryLightColor: Kirigami.Theme.alternateBackgroundColor
    property color primaryDarkColor: Kirigami.Theme.alternateBackgroundColor
    property color textOnPrimaryColor: Kirigami.Theme.textColor
    property color secondaryColor: Kirigami.Theme.backgroundColor
    property color secondaryLightColor: Kirigami.Theme.alternateBackgroundColor
    property color secondaryDarkColor: Kirigami.Theme.alternateBackgroundColor
    property color textOnSecondaryColor: Kirigami.Theme.textColor

    property color keyboardBackgroundColor: primaryColor
    property color normalKeyBackgroundColor: primaryDarkColor
    property color highlightedKeyBackgroundColor: primaryLightColor
    property color capsLockKeyAccentColor: secondaryColor
    property color modeKeyAccentColor: textOnPrimaryColor
    property color keyTextColor: textOnPrimaryColor
    property color keySmallTextColor: textOnPrimaryColor
    property color popupBackgroundColor: secondaryColor
    property color popupBorderColor: secondaryLightColor
    property color popupTextColor: textOnSecondaryColor
    property color popupHighlightColor: secondaryLightColor
    property color selectionListTextColor: textOnPrimaryColor
    property color selectionListSeparatorColor: primaryLightColor
    property color selectionListBackgroundColor: primaryColor
    property color navigationHighlightColor: "yellow"

    readonly property real buttonRadius: Kirigami.Units.cornerRadius

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
            implicitWidth: 80 * keyIconScale
            implicitHeight: 80 * keyIconScale
            source: "settings-configure"
        }
    }

    keyboardDesignWidth: {
        if (Screen.width < 500) {
            // Phone mode
            return 1000;
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
            return 700;
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
        color: keyboardBackgroundColor
    }

    keyPanel: BreezeKeyPanel {
        id: keyPanel

        Rectangle {
            id: keyBackground
            radius: buttonRadius
            color: control && control.highlighted ? highlightedKeyBackgroundColor : normalKeyBackgroundColor
            anchors.fill: keyPanel
            anchors.margins: keyBackgroundMargin
            QQC2.Label {
                id: keySmallText
                text: control.smallText
                visible: control.smallTextVisible
                color: keySmallTextColor
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: keyContentMargin / 3
                font {
                    family: fontFamily
                    weight: Font.Light
                    pixelSize: 40 * scaleHint
                    capitalization: control.uppercased ? Font.AllUppercase : Font.MixedCase
                }
            }
            Loader {
                id: loader_settingsIcon
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: keyContentMargin / 3
            }
            QQC2.Label {
                id: keyText
                text: control.displayText
                color: keyTextColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: control.displayText.length > 1 ? Text.AlignVCenter : Text.AlignBottom
                anchors.fill: parent
                anchors.leftMargin: keyContentMargin
                anchors.topMargin: keyContentMargin
                anchors.rightMargin: keyContentMargin
                anchors.bottomMargin: keyContentMargin
                font {
                    family: fontFamily
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
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: keyBackground
                    opacity: 0.75
                }
                PropertyChanges {
                    target: keyText
                    opacity: 0.5
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: keyBackground
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

        Rectangle {
            id: backspaceKeyBackground
            radius: buttonRadius
            color: control && control.highlighted ? highlightedKeyBackgroundColor : normalKeyBackgroundColor
            anchors.fill: backspaceKeyPanel
            anchors.margins: keyBackgroundMargin
            Kirigami.Icon {
                id: backspaceKeyIcon
                anchors.centerIn: parent
                implicitHeight: 88 * keyIconScale
                implicitWidth: implicitHeight
                source: "edit-clear-symoblic"
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: backspaceKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: backspaceKeyIcon
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: backspaceKeyBackground
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

        Rectangle {
            id: languageKeyBackground
            radius: buttonRadius
            color: control && control.highlighted ? highlightedKeyBackgroundColor : normalKeyBackgroundColor
            anchors.fill: languageKeyPanel
            anchors.margins: keyBackgroundMargin

            Kirigami.Icon {
                id: languageKeyIcon
                anchors.centerIn: parent
                implicitHeight: 96 * keyIconScale
                source: "globe"
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: languageKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: languageKeyIcon
                    opacity: 0.75
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: languageKeyBackground
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

        Rectangle {
            id: enterKeyBackground
            radius: buttonRadius
            color: control && control.highlighted ? highlightedKeyBackgroundColor : normalKeyBackgroundColor
            anchors.fill: enterKeyPanel
            anchors.margins: keyBackgroundMargin
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
                implicitHeight: enterKeyIconSize.height * keyIconScale
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
                color: keyTextColor
                font {
                    family: fontFamily
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
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: enterKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: enterKeyIcon
                    opacity: 0.6
                }
                PropertyChanges {
                    target: enterKeyText
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: enterKeyBackground
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

        Rectangle {
            id: hideKeyBackground
            radius: buttonRadius
            color: control && control.highlighted ? highlightedKeyBackgroundColor : normalKeyBackgroundColor
            anchors.fill: hideKeyPanel
            anchors.margins: keyBackgroundMargin
            Kirigami.Icon {
                id: hideKeyIcon
                anchors.centerIn: parent
                implicitHeight: 96 * keyIconScale
                source: "input-keyboard-virtual-hide-symbolic"
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: hideKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: hideKeyIcon
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: hideKeyBackground
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

        Rectangle {
            id: shiftKeyBackground
            radius: buttonRadius
            color: control && control.highlighted ? highlightedKeyBackgroundColor : normalKeyBackgroundColor
            anchors.fill: shiftKeyPanel
            anchors.margins: keyBackgroundMargin
            Kirigami.Icon {
                id: shiftKeyIcon
                anchors.centerIn: parent
                implicitHeight: 134 * keyIconScale
                source: "keyboard-caps-disabled-symbolic"
            }
            states: [
                State {
                    name: "capsLockActive"
                    when: InputContext.capsLockActive
                    PropertyChanges {
                        target: shiftKeyBackground
                        color: capsLockKeyAccentColor
                    }
                    PropertyChanges {
                        target: shiftKeyIcon
                        source: "keyboard-caps-locked-symbolic"
                    }
                },
                State {
                    name: "shiftActive"
                    when: InputContext.shiftActive
                    PropertyChanges {
                        target: shiftKeyIcon
                        source: "keyboard-caps-enabled-symbolic"
                    }
                }
            ]
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: shiftKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: shiftKeyIcon
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: shiftKeyBackground
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

        Rectangle {
            id: spaceKeyBackground
            radius: buttonRadius
            color: control && control.highlighted ? highlightedKeyBackgroundColor : normalKeyBackgroundColor
            anchors.fill: spaceKeyPanel
            anchors.margins: keyBackgroundMargin
            QQC2.Label {
                id: spaceKeyText
                text: Qt.locale(InputContext.locale).nativeLanguageName
                color: keyTextColor
                opacity: inputLocaleIndicatorOpacity
                Behavior on opacity { PropertyAnimation { duration: 250 } }
                anchors.centerIn: parent
                font {
                    family: fontFamily
                    weight: Font.Light
                    pixelSize: 35 * scaleHint
                }
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: spaceKeyBackground
                    opacity: 0.80
                }
            },
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

        Rectangle {
            id: symbolKeyBackground
            radius: buttonRadius
            color: control && control.highlighted ? highlightedKeyBackgroundColor : normalKeyBackgroundColor
            anchors.fill: symbolKeyPanel
            anchors.margins: keyBackgroundMargin
            QQC2.Label {
                id: symbolKeyText
                text: control.displayText
                color: keyTextColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent
                anchors.margins: keyContentMargin
                font {
                    family: fontFamily
                    weight: Font.Light
                    pixelSize: 40 * scaleHint
                    capitalization: Font.AllUppercase
                }
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: symbolKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: symbolKeyText
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: symbolKeyBackground
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

        Rectangle {
            id: modeKeyBackground
            radius: buttonRadius
            color: control && control.highlighted ? highlightedKeyBackgroundColor : normalKeyBackgroundColor
            anchors.fill: modeKeyPanel
            anchors.margins: keyBackgroundMargin
            QQC2.Label {
                id: modeKeyText
                text: control.displayText
                color: keyTextColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent
                anchors.margins: keyContentMargin
                font {
                    family: fontFamily
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
                color: modeKeyAccentColor
                radius: buttonRadius
                visible: control.mode
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: modeKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: modeKeyText
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: modeKeyBackground
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

        Rectangle {
            id: hwrKeyBackground
            radius: buttonRadius
            color: control && control.highlighted ? highlightedKeyBackgroundColor : normalKeyBackgroundColor
            anchors.fill: handwritingKeyPanel
            anchors.margins: keyBackgroundMargin
            Kirigami.Icon {
                id: hwrKeyIcon
                anchors.centerIn: parent
                implicitHeight: 127 * keyIconScale
                source: (keyboard.handwritingMode ? "edit-select-text-symbolic" : "draw-freehand-symbolic")
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: hwrKeyBackground
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
                    target: hwrKeyBackground
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
            color: popupBackgroundColor
            radius: buttonRadius
            readonly property int largeTextHeight: Math.round(height / 3 * 2)
            readonly property int smallTextHeight: Math.round(height / 3)
            readonly property int smallTextMargin: Math.round(3 * scaleHint)
            QQC2.Label {
                id: characterPreviewText
                color: popupTextColor
                text: characterPreview.text
                fontSizeMode: Text.VerticalFit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                height: characterPreviewBackground.largeTextHeight
                font {
                    family: fontFamily
                    weight: Font.Light
                    pixelSize: 82 * scaleHint
                }
            }
            QQC2.Label {
                color: popupTextColor
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
                    family: fontFamily
                    weight: Font.Light
                    pixelSize: 62 * scaleHint
                }
            }
            QQC2.Label {
                color: popupTextColor
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
                    family: fontFamily
                    weight: Font.Light
                    pixelSize: 62 * scaleHint
                }
            }
            QQC2.Label {
                color: popupTextColor
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
                    family: fontFamily
                    weight: Font.Light
                    pixelSize: 62 * scaleHint
                }
            }
            QQC2.Label {
                color: popupTextColor
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
                    family: fontFamily
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
            color: popupTextColor
            opacity: 0.8
            font {
                family: fontFamily
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
        color: popupHighlightColor
        radius: buttonRadius
    }
    alternateKeysListBackground: Item {
        Rectangle {
            readonly property real margin: 20 * scaleHint
            x: -margin
            y: -margin
            width: parent.width + 2 * margin
            height: parent.height + 2 * margin
            radius: buttonRadius
            color: popupBackgroundColor
            border {
                width: 1
                color: popupBorderColor
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
            color: selectionListTextColor
            opacity: 0.9
            font {
                family: fontFamily
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
            color: selectionListSeparatorColor
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
        color: selectionListBackgroundColor
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
        border.color: navigationHighlightColor
        border.width: 5
    }

    traceInputKeyPanelDelegate: TraceInputKeyPanel {
        id: traceInputKeyPanel
        traceMargins: keyBackgroundMargin
        Rectangle {
            id: traceInputKeyPanelBackground
            radius: buttonRadius
            color: normalKeyBackgroundColor
            anchors.fill: traceInputKeyPanel
            anchors.margins: keyBackgroundMargin
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
                color: keyTextColor
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: keyContentMargin
                font {
                    family: fontFamily
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
            color: popupTextColor
            opacity: 0.8
            font {
                family: fontFamily
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
        color: popupBackgroundColor
        border {
            width: 1
            color: popupBorderColor
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
            color: popupTextColor
            opacity: 0.8
            font {
                family: fontFamily
                weight: Font.Light
                pixelSize: 44 * scaleHint
            }
        }
        TextMetrics {
            id: languageNameTextMetrics
            font {
                family: fontFamily
                weight: Font.Light
                pixelSize: 44 * scaleHint
            }
            text: "X"
        }
        TextMetrics {
            id: languageNameFormatter
            font {
                family: fontFamily
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
        color: popupHighlightColor
        radius: buttonRadius
    }

    languageListBackground: Rectangle {
        color: popupBackgroundColor
        border {
            width: 1
            color: popupBorderColor
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
        readonly property real iconWidth: 96 * keyIconScale
        readonly property real iconHeight: 96 * keyIconScale
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
            radius: buttonRadius
            color: popupBackgroundColor
            border {
                width: 1
                color: popupBorderColor
            }
        }
    }

    functionPopupListHighlight: Rectangle {
        color: popupHighlightColor
        radius: buttonRadius
    }
}
