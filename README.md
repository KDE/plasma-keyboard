# Plasma Keyboard

The plasma-keyboard is a virtual keyboard based on [Qt Virtual Keyboard](https://doc.qt.io/qt-6/qtvirtualkeyboard-overview.html) designed to integrate in Plasma.


## Build and install

```sh
mkdir build && cd build
cmake ..
make && make install
```

## Usage with kwin

To actually leverage plasma-keyboard with kwin, you need to start kwin with the `--inputmethod` argument set to
`plasma-keyboard`, e.g.:

```sh
kwin_wayland --inputmethod plasma-keyboard
```

## Troubleshooting

If the Virtual Keyboard does not show up add `KWIN_IM_SHOW_ALWAYS=1` to kwin's environment variables.

## Top Level vs. Floating Mode

By default, the Virtual Keyboard will use a top level window that is sized to occupy the full screen width. The height
is automatically deduced based on the inherent keyboard size ratio. The keyboard will be shown at the bottom of the
screen.

In the top level window mode, all other windows will be automatically resized to fit into the remaining space at the
top of the screen. While this mode is perfect for portrait screens, the keyboard may be too large for widescreens in
landscape orientation.

If you instead want to use a floating virtual keyboard, you can add `QT_WAYLAND_INPUT_PANEL_TOPLEVEL=0` to kwin's
environment variables. This will enable the floating mode, where the keyboard overlaps other windows. The width of the
floating window is also reduced to half the screen width. The floating keyboard window will be shown horizontally
centered at the bottom of the screen.
