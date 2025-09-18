<!--
  - SPDX-FileCopyrightText: None
  - SPDX-License-Identifier: CC0-1.0
-->

# Plasma Keyboard

The plasma-keyboard is a virtual keyboard based on [Qt Virtual Keyboard](https://doc.qt.io/qt-6/qtvirtualkeyboard-overview.html) designed to integrate in Plasma.


## Build and install

```sh
mkdir build && cd build
cmake ..
make && make install
```

## Troubleshooting

If the Virtual Keyboard does not show up add `KWIN_IM_SHOW_ALWAYS=1` to kwin's environment variables.

To use Qt's builtin keyboard layouts rather than the ones we supply in `plasma-keyboard`, set `PLASMA_KEYBOARD_USE_QT_LAYOUTS=1` to kwin's environment variables.
