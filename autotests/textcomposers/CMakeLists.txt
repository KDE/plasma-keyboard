# SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

if(PLASMA_KEYBOARD_ANTHY_ENABLED)
    ecm_add_test(anthytextcomposertest.cpp
        TEST_NAME anthytextcomposertest
        LINK_LIBRARIES plasma-keyboard-virtualkeyboard Qt::Gui Qt::Qml Qt::Test
    )
endif()

if(PLASMA_KEYBOARD_LIBPINYIN_ENABLED)
    ecm_add_test(pinyintextcomposertest.cpp
        TEST_NAME pinyintextcomposertest
        LINK_LIBRARIES plasma-keyboard-virtualkeyboard Qt::Gui Qt::Qml Qt::Test
    )
endif()

if(PLASMA_KEYBOARD_LIBCHEWING_ENABLED)
    ecm_add_test(zhuyintextcomposertest.cpp
        TEST_NAME zhuyintextcomposertest
        LINK_LIBRARIES plasma-keyboard-virtualkeyboard Qt::Gui Qt::Qml Qt::Test
    )
endif()

if(PLASMA_KEYBOARD_LIBHANGUL_ENABLED)
    ecm_add_test(hangultextcomposertest.cpp
        TEST_NAME hangultextcomposertest
        LINK_LIBRARIES plasma-keyboard-virtualkeyboard Qt::Gui Qt::Qml Qt::Test
    )
endif()
