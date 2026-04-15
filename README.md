<!--
  - SPDX-FileCopyrightText: None
  - SPDX-License-Identifier: CC0-1.0
-->

# Plasma Keyboard

The plasma-keyboard is a virtual keyboard and input engine designed to integrate in Plasma.

It uses the input-method-v1 Wayland protocol to communicate with the compositor to function as an input method. It also can leverage KWin's fake-input protocol in order to emulate keyboard keys (ex. Meta, Ctrl).

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

The keyboard layouts are located in the [layouts](/layouts) folder.

See the existing layouts there for examples on creating layout packages. They are installed (roughly) to `/usr/share/plasma/keyboard/keyboardpackages`.

See the component library for making layouts for documentation at [virtualkeyboard/components](/virtualkeyboard/components).

### Text composers

A default text composer is provided that emits keys directly to the input engine (see [directtextcomposer](virtualkeyboard/textcomposers/directtextcomposer.h)) which works for most languages.

Some languages require some extra processing in preedit to form words, and so have their own text composers:

Text composers are located in [virtualkeyboard/textcomposers](/virtualkeyboard/textcomposers).

## Troubleshooting

KWin by default only shows the keyboard when a text field is interacted with by touch. Set `KWIN_IM_SHOW_ALWAYS=1` when starting KWin (or the login session) in order to force the keyboard to always pop up.
