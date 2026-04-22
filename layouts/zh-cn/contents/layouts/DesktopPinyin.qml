// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import org.kde.plasma.keyboard.layouts.defaultplugin as SharedLayouts
import org.kde.plasma.keyboard.virtualkeyboard

SharedLayouts.Desktop {
    textComposer: PinyinTextComposer { inputMode: "pinyin" }
    inputModeKeyModes: [
        { value: "pinyin", label: "中" },
        { value: "latin", label: "英" }
    ]
}
