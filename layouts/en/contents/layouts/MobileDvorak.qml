// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import org.kde.plasma.keyboard.layouts.defaultplugin as SharedLayouts
import org.kde.plasma.keyboard.virtualkeyboard.components
import "shared" as Shared

KeyboardLayoutDefinition {
    symbols: Shared.Symbols {}
    numbers: Shared.Numbers {}
    digits: Shared.Digits {}
    dialpad: Shared.Dialpad {}

    SharedLayouts.Dvorak {}
}
