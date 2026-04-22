# SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_subdirectory(defaultplugin)

plasma_install_package(en org.kde.plasma.keyboard.en keyboard/keyboardpackages)

if(PLASMA_KEYBOARD_ANTHY_ENABLED)
    plasma_install_package(ja org.kde.plasma.keyboard.ja keyboard/keyboardpackages)
endif()

if(PLASMA_KEYBOARD_LIBHANGUL_ENABLED)
    plasma_install_package(ko org.kde.plasma.keyboard.ko keyboard/keyboardpackages)
endif()

if(PLASMA_KEYBOARD_LIBPINYIN_ENABLED)
    plasma_install_package(zh-cn org.kde.plasma.keyboard.zh-cn keyboard/keyboardpackages)
endif()

if(PLASMA_KEYBOARD_LIBCHEWING_ENABLED)
    plasma_install_package(zh-tw org.kde.plasma.keyboard.zh-tw keyboard/keyboardpackages)
endif()
