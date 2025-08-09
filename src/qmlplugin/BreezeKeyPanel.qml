// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Styles

import org.kde.plasma.keyboard
import org.kde.kirigami as Kirigami

KeyPanel {
    id: root

    default property alias contentItem: visualContainer.contentItem

    property alias background: visualContainer.background

    property real padding: BreezeConstants.keyBackgroundMargin

    property real radius: BreezeConstants.buttonRadius

    property color color: {
        if (control && control.pressed) {
            return BreezeConstants.normalKeyPressedBackgroundColor;
        } else if (control && control.highlighted) {
            return BreezeConstants.highlightedKeyBackgroundColor;
        }
        return BreezeConstants.normalKeyBackgroundColor;
    }

    soundEffect: PlasmaKeyboardSettings.soundEnabled ? Qt.resolvedUrl('qrc:///sounds/keyboard_tick2_quiet.wav') : ''

    QQC2.Control {
        id: visualContainer
        anchors.fill: parent
        anchors.margins: root.padding

        background: Kirigami.ShadowedRectangle {
            color: root.color
            radius: root.radius

            // Shadow
            shadow.color: Qt.rgba(0, 0, 0, 0.2)
            shadow.size: 3
            shadow.yOffset: 1
        }
    }

    // Implement key vibration
    Connections {
        target: root.control
        function onPressedChanged() {
            if (root.control.pressed && PlasmaKeyboardSettings.vibrationEnabled) {
                Vibration.vibrate(PlasmaKeyboardSettings.vibrationMs);
            }
        }
    }
}
