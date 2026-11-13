/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import org.kde.ki18n
import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    KI18nContext {
        id: _tr
        translationDomain: "kcm_plasmakeyboard"
    }

    title: _tr.i18n("Add Layout")

    KeyboardLayoutSelectorListView {}
}
