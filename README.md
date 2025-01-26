# qvk

qvk is a virtual keyboard based on [Qt Virtual Keyboard](https://doc.qt.io/qt-6/qtvirtualkeyboard-overview.html).


## Build and install

```sh
mkdir build && cd build
cmake ..
make && make install
```

## Troubleshooting

If the Virtual Keyboard does not show up add KWIN_IM_SHOW_ALWAYS=1 to your environment variable.
