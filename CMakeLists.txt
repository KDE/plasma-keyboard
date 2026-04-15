# SPDX-FileCopyrightText: 2024 Aleix Pol <aleixpol@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION "6.6.80")

project(plasma-keyboard VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_DEP_VERSION "6.5.90")
set(QT_MIN_VERSION "6.10.0")
set(KF6_MIN_VERSION "6.22.0")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs) # Should be included first so its variables are available to other modules.

include(ECMDeprecationSettings)
include(ECMInstallIcons)
include(ECMQmlModule)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)

include(FeatureSummary)

include(KDEClangFormat)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEGitCommitHooks)

option(PLASMA_KEYBOARD_SOUNDS_ENABLED "Whether to enable sound effect support, requires GPLv3 compliance" ON)
option(PLASMA_KEYBOARD_VIBRATION_ENABLED "Whether to enable haptic feedback via vibrations, requires GPLv3 compliance" ON)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    VirtualKeyboard
    WaylandClient
)

if(Qt6_VERSION VERSION_GREATER_EQUAL "6.10.0")
    find_package(Qt6GuiPrivate ${QT_MIN_VERSION} REQUIRED NO_MODULE)
    find_package(Qt6WaylandClientPrivate ${QT_MIN_VERSION} REQUIRED NO_MODULE)
endif()

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    I18n
    KCMUtils
    Package
    Config
    Crash
)

find_package(PlasmaQuick ${PROJECT_DEP_VERSION} REQUIRED)

ecm_set_disabled_deprecation_versions(QT 6.11.0
    KF 6.23.0
)

ecm_setup_version(${PROJECT_VERSION} VARIABLE_PREFIX PLASMA_KEYBOARD VERSION_HEADER plasma_keyboard_version.h)

find_package(Wayland 1.2 REQUIRED COMPONENTS Client)
find_package(QtWaylandScanner REQUIRED ${QT_MIN_VERSION})
find_package(WaylandProtocols 1.19 REQUIRED)

add_subdirectory(virtualkeyboard)
add_subdirectory(kcm)
add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()

ki18n_install(po)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
