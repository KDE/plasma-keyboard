# SPDX-FileCopyrightText: 2026 Aleix Pol <aleixpol@kde.org>
# SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>
#
# SPDX-License-Identifier: BSD-2-Clause

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Test WaylandCompositor)
include(ECMAddTests)

file(GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/plasmakeyboard_test_config.h
    CONTENT "#pragma once\n#define PLASMA_KEYBOARD_BINARY_PATH \"$<TARGET_FILE:plasma-keyboard>\"\n"
)

add_library(mock_compositor OBJECT
    mockcompositor.cpp
    plasmakeyboardtestcase.cpp
)
set_target_properties(mock_compositor PROPERTIES AUTOMOC ON)
target_include_directories(mock_compositor PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(mock_compositor PUBLIC
    KF6::ConfigCore
    Qt::Core
    Qt::Gui
    Qt::GuiPrivate
    Qt::Test
    Qt::WaylandCompositor
    Wayland::Server
)

qt6_generate_wayland_protocol_server_sources(mock_compositor
    FILES
        ${WaylandProtocols_DATADIR}/unstable/input-method/input-method-unstable-v1.xml
        ${Wayland_DATADIR}/wayland.xml
)

ecm_add_test(diacriticstest.cpp
    LINK_LIBRARIES mock_compositor Qt::Test
)
