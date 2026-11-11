# SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

add_definitions(-DQT_NO_KEYWORDS)

kcmutils_add_qml_kcm(kcm_textexpansion
    URI "org.kde.kcm.textexpansion"
)

kconfig_add_kcfg_files(kcm_textexpansion GENERATE_MOC ../plasmakeyboardsettings.kcfgc)

target_sources(kcm_textexpansion PRIVATE
    textexpansionkcm.cpp
    textexpansionkcm.h
    textexpansionmodel.cpp
    textexpansionmodel.h
)

target_link_libraries(kcm_textexpansion PRIVATE
    Qt::Core
    Qt::Qml
    KF6::CoreAddons
    KF6::I18n
    KF6::KCMUtilsQuick
    KF6::ConfigCore
    KF6::ConfigGui
)
