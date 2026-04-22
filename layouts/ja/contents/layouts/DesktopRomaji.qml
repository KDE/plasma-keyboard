// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import org.kde.plasma.keyboard.layouts.defaultplugin as SharedLayouts
import org.kde.plasma.keyboard.virtualkeyboard

SharedLayouts.Desktop {
    textComposer: AnthyTextComposer { inputMode: "hiragana" }
    inputModeKeyModes: [
        { value: "hiragana", label: "あ" },
        { value: "latin", label: "A" }
    ]
}
