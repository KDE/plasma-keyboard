/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.kemoji as KEmoji
import org.kde.plasma.keyboard
import org.kde.plasma.keyboard.virtualkeyboard.components

KEmoji.EmojiDelegate {
    id: root

    property bool navigationActive: false

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    topInset: 0
    bottomInset: 0
    leftInset: 0
    rightInset: 0

    // Add our own styled key background
    background: Item {
        Rectangle {
            anchors.fill: parent
            radius: BreezeConstants.buttonRadius
            color: {
                if (root.down) {
                    return BreezeConstants.normalKeyPressedBackgroundColor;
                }
                if (root.highlighted) {
                    return BreezeConstants.highlightedKeyBackgroundColor;
                }
                return "transparent";
            }
        }

        NavigationHighlight {
            anchors.fill: parent
            visible: root.navigationActive
        }
    }
}
