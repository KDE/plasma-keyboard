<!--
  - SPDX-FileCopyrightText: None
  - SPDX-License-Identifier: CC0-1.0
-->

# Plasma Keyboard

The plasma-keyboard is a virtual keyboard and input engine designed to integrate in Plasma.

It uses the input-method-v1 Wayland protocol to communicate with the compositor to function as an input method. It also can leverage KWin's fake-input protocol in order to emulate keyboard keys (ex. Meta, Ctrl).

## Build and install

```sh
mkdir build && cd build
cmake ..
make && make install
```

## Install using the flatpak nightly repository

https://cdn.kde.org/flatpak/plasma-keyboard-nightly/org.kde.plasma.keyboard.flatpakref

See also: https://userbase.kde.org/Tutorials/Flatpak#Nightly_KDE_apps

## Layouts

The keyboard layouts are located in the [layouts](/layouts) folder.

See the existing layouts there for examples on creating layout packages. They are installed (roughly) to `/usr/share/plasma/keyboard/keyboardpackages`.

See the component library for making layouts for documentation at [virtualkeyboard/components](/virtualkeyboard/components).

### Text composers

A default text composer is provided that emits keys directly to the input engine (see [directtextcomposer](virtualkeyboard/textcomposers/directtextcomposer.h)) which works for most languages.

Some languages require some extra processing in preedit to form words, and so have their own text composers:
- Chinese/Pinyin (via libpinyin)
- Chinese/Zhuyin (via libchewing)
- Japanese (via anthy)
- Korean (via libhangul)

Text composers are located in [virtualkeyboard/textcomposers](/virtualkeyboard/textcomposers).

## Troubleshooting

KWin by default only shows the keyboard when a text field is interacted with by touch. Set `KWIN_IM_SHOW_ALWAYS=1` when starting KWin (or the login session) in order to force the keyboard to always pop up.
