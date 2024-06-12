cmake_minimum_required(VERSION 3.16)

project(qvk VERSION "1.0")

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMQmlModule)
include(KDEClangFormat)
include(FeatureSummary)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Gui
    VirtualKeyboard
    WaylandClient
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    I18n
)

find_package(Wayland 1.2 REQUIRED COMPONENTS Client)
find_package(QtWaylandScanner REQUIRED ${QT_MIN_VERSION})
find_package(WaylandProtocols 1.19 REQUIRED)

add_subdirectory(src)

ki18n_install(po)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
