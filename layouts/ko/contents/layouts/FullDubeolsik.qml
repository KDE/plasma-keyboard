// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import org.kde.plasma.keyboard.layouts.defaultplugin as SharedLayouts
import org.kde.plasma.keyboard.virtualkeyboard

SharedLayouts.FullDesktop {
    textComposer: HangulTextComposer { inputMode: "hangul" }
    sendShiftDirectKeyEvents: textComposer.inputMode !== "hangul"
    inputModeKeyModes: [
        { value: "hangul", label: "\uD55C" },
        { value: "latin", label: "A" }
    ]
    keyTextMap: textComposer.inputMode === "hangul" ? {
        q: "\u3142", w: "\u3148", e: "\u3137", r: "\u3131", t: "\u3145", y: "\u315B", u: "\u3155", i: "\u3151", o: "\u3150", p: "\u3154",
        a: "\u3141", s: "\u3134", d: "\u3147", f: "\u3139", g: "\u314E", h: "\u3157", j: "\u3153", k: "\u314F", l: "\u3163",
        z: "\u314B", x: "\u314C", c: "\u314A", v: "\u314D", b: "\u3160", n: "\u315C", m: "\u3161"
    } : ({})
    shiftedKeyTextMap: textComposer.inputMode === "hangul" ? {
        q: "\u3143", w: "\u3149", e: "\u3138", r: "\u3132", t: "\u3146", o: "\u3152", p: "\u3156"
    } : ({})
}
