// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import org.kde.plasma.keyboard.layouts.defaultplugin as SharedLayouts
import org.kde.plasma.keyboard.virtualkeyboard

SharedLayouts.Desktop {
    textComposer: ZhuyinTextComposer { inputMode: "zhuyin" }
    inputModeKeyModes: [
        { value: "zhuyin", label: "中" },
        { value: "latin", label: "英" }
    ]
    keyTextMap: textComposer.inputMode === "zhuyin" ? {
        q: "\u3106", w: "\u310A", e: "\u310D", r: "\u3110", t: "\u3114", y: "\u3117", u: "\u3127", i: "\u311B", o: "\u311F", p: "\u3123",
        a: "\u3107", s: "\u310B", d: "\u310E", f: "\u3111", g: "\u3115", h: "\u3118", j: "\u3128", k: "\u311C", l: "\u3120",
        z: "\u3108", x: "\u310C", c: "\u310F", v: "\u3112", b: "\u3116", n: "\u3119", m: "\u3129",
        "'": "\u3124", ",": "\u311D", ".": "\u3121", "?": "\u3125"
    } : ({})
}
