# SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_library(plasma-keyboard-virtualkeyboard STATIC
    inputbackend.cpp
    inputbackend.h
    inputengine.cpp
    inputengine.h
    textcomposers/abstracttextcomposer.cpp
    textcomposers/abstracttextcomposer.h
    textcomposers/directtextcomposer.cpp
    textcomposers/directtextcomposer.h
    keyboardcontroller.cpp
    keyboardcontroller.h
    keyboardlayoutmetadata.cpp
    keyboardpackageresolver.cpp
    keyboardpackageresolver.h
    virtualkeyboardattached.cpp
    virtualkeyboardattached.h
    virtualkeyboardcontext.cpp
    virtualkeyboardcontext.h
    wordcandidatelistmodel.cpp
    wordcandidatelistmodel.h
)

target_include_directories(plasma-keyboard-virtualkeyboard
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/textcomposers
    PRIVATE
        ${CMAKE_BINARY_DIR}
        ${CMAKE_BINARY_DIR}/src
)

target_link_libraries(plasma-keyboard-virtualkeyboard
    PUBLIC
        Qt::Core
    PRIVATE
        KF6::Package
        Qt::Gui
        Qt::GuiPrivate
        Qt::Qml
        Qt::Quick
)

if(PLASMA_KEYBOARD_LIBPINYIN_ENABLED)
    target_sources(plasma-keyboard-virtualkeyboard PRIVATE
        textcomposers/pinyintextcomposer.cpp
        textcomposers/pinyintextcomposer.h
    )
    target_link_libraries(plasma-keyboard-virtualkeyboard PRIVATE PkgConfig::LIBPINYIN)
endif()

if(PLASMA_KEYBOARD_LIBCHEWING_ENABLED)
    target_sources(plasma-keyboard-virtualkeyboard PRIVATE
        textcomposers/zhuyintextcomposer.cpp
        textcomposers/zhuyintextcomposer.h
    )
    target_link_libraries(plasma-keyboard-virtualkeyboard PRIVATE PkgConfig::LIBCHEWING)
endif()

if(PLASMA_KEYBOARD_LIBHANGUL_ENABLED)
    target_sources(plasma-keyboard-virtualkeyboard PRIVATE
        textcomposers/hangultextcomposer.cpp
        textcomposers/hangultextcomposer.h
    )
    target_link_libraries(plasma-keyboard-virtualkeyboard PRIVATE PkgConfig::LIBHANGUL)
endif()

if(PLASMA_KEYBOARD_ANTHY_ENABLED)
    target_sources(plasma-keyboard-virtualkeyboard PRIVATE
        textcomposers/anthytextcomposer.cpp
        textcomposers/anthytextcomposer.h
    )
    target_link_libraries(plasma-keyboard-virtualkeyboard PRIVATE PkgConfig::LIBANTHY ${LIBANTHYINPUT_LIBRARY})
endif()

ecm_add_qml_module(plasma-keyboard-virtualkeyboard
    URI org.kde.plasma.keyboard.virtualkeyboard
    GENERATE_PLUGIN_SOURCE
    DEPENDENCIES
        QtQuick
)

ecm_add_qml_module(keyboardvirtualkeyboardcomponents
    URI org.kde.plasma.keyboard.virtualkeyboard.components
    GENERATE_PLUGIN_SOURCE
    DEPENDENCIES
        QtQuick
        QtQuick.Controls
        org.kde.kirigami
        org.kde.plasma.keyboard.virtualkeyboard
)

set_source_files_properties(
    components/BreezeConstants.qml
    components/keys/FlickKeyPrivate.qml
    PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

ecm_target_qml_sources(keyboardvirtualkeyboardcomponents SOURCES
    components/BreezeConstants.qml
    components/PopupBackground.qml
    components/AlternativeKeysPopup.qml
    components/FlickPreviewPopup.qml
    components/Keyboard.qml
    components/KeyboardNavigation.qml
    components/KeyboardStrip.qml
    components/KeyboardColumn.qml
    components/KeyboardLayoutLoader.qml
    components/KeyboardLayout.qml
    components/LanguagePopup.qml
    components/KeyboardRow.qml
    components/PreeditBubble.qml
    components/keys/ActionKey.qml
    components/keys/AbstractKey.qml
    components/keys/BackspaceKey.qml
    components/keys/ChangeLanguageKey.qml
    components/keys/EnterKey.qml
    components/keys/FillerKey.qml
    components/keys/FlickKey.qml
    components/keys/FlickKeyPrivate.qml
    components/keys/HideKeyboardKey.qml
    components/keys/InputModeKey.qml
    components/keys/Key.qml
    components/keys/KeyMouseArea.qml
    components/keys/KeysymKey.qml
    components/keys/ShiftKey.qml
    components/keys/SpaceKey.qml
    components/keys/SymbolModeKey.qml
)

target_link_libraries(keyboardvirtualkeyboardcomponents
    PRIVATE
        Qt::Qml
        Qt::Quick
)

ecm_finalize_qml_module(plasma-keyboard-virtualkeyboard)
ecm_finalize_qml_module(keyboardvirtualkeyboardcomponents)
