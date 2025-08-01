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
