<!--
  - SPDX-FileCopyrightText: None
  - SPDX-License-Identifier: CC0-1.0
-->

# Plasma Keyboard

The plasma-keyboard is a virtual keyboard based on [Qt Virtual Keyboard](https://doc.qt.io/qt-6/qtvirtualkeyboard-overview.html) designed to integrate in Plasma.

It wraps Qt Virtual Keyboard in a window, and uses the input-method-v1 Wayland protocol to communicate with the compositor to function as an input method.

## Install using the flatpak nightly repository

https://cdn.kde.org/flatpak/plasma-keyboard-nightly/org.kde.plasma.keyboard.flatpakref

See also: https://userbase.kde.org/Tutorials/Flatpak#Nightly_KDE_apps

## Development

Recommended methods for development are to either use
[KDE Linux](https://linux.kde.org/) (as your OS or in a VM), or to build the
Flatpak version of plasma-keyboard.

### KDE Linux (recommended)

Follow the
[instructions](https://linux.kde.org/docs/kde-dev/#build-kde-software-thats-shipped-on-the-base-image)
to set up a development environment for KDE software.

Build plasma-keyboard once to clone the source locally:

```bash
kde-builder plasma-keyboard
```

After making changes to the code the workflow is: rebuild, refresh sysext, and restart plasma-keyboard. This
script can be used to do that:

```bash
#!/usr/bin/env bash

set -e

# Rebuild plasma-keyboard and refresh the sysext
kde-builder --no-src plasma-keyboard && systemctl --user daemon-reload && sudo systemd-sysext refresh --always-refresh=yes && systemctl restart --user plasma-plasmashell.service

# Disable plasma-keyboard
kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod ''

# Enable plasma-keyboard with the newly built changes
kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod '/usr/share/applications/org.kde.plasma.keyboard.desktop'
```

### Flatpak

It is also possible to build plasma-keyboard as a Flatpak. This is the
recommended method for development on distributions other than KDE Linux, as we
don't want to install the development version of plasma-keyboard on a
traditional Linux distribution (which may break the system).

Clone the repository and make changes to the code, then build and install the
development version of the Flatpak with your changes by running the following
from the repository root (or save as a script and run it):

```bash
#!/usr/bin/env bash

# Build and install the flatpak
flatpak-builder --user --install --force-clean build-flatpak .flatpak-manifest.json

# Disable plasma-keyboard if it is already running
killall plasma-keyboard; kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod ''

# Enable plasma-keyboard with the newly built changes
kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod '$HOME/.local/share/flatpak/exports/share/applications/org.kde.plasma.keyboard.desktop'

```

### Building from source manually

```sh
mkdir build && cd build
cmake ..
make && make install
```

### Troubleshooting

Join the [KDE Matrix chat](https://community.kde.org/Matrix) so we can help you
get started with development! We have a room specifically for input handling:
[#kde-input:kde.org](https://matrix.to/#/#kde-input:kde.org)

## Layouts

The keyboard layouts are located in the [src/layouts](/src/layouts) folder.

They are forked from Qt's [layouts](https://github.com/qt/qtvirtualkeyboard/tree/dev/src/layouts), with modifications that we want for Plasma. Please view the official [Qt documentation](https://doc.qt.io/qt-6/qtvirtualkeyboard-overview.html#adding-new-keyboard-layouts) for a guide on how to create and modify keyboard layouts.

To use Qt's built-in keyboard layouts rather than the ones we supply in `plasma-keyboard`, set `PLASMA_KEYBOARD_USE_QT_LAYOUTS=1` when starting KWin (or the login session).

## Troubleshooting

KWin by default only shows the keyboard when a text field is interacted with by touch. Set `KWIN_IM_SHOW_ALWAYS=1` when starting KWin (or the login session) in order to force the keyboard to always pop up.

