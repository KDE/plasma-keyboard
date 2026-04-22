#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-or-later
# SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

set -e

export GIT_CLONE_ARGS="--depth 1 --single-branch"
export GIT_FETCH_ARGS="--depth 1"
export FLATPAK_DIR="$(readlink -f "$(dirname "$0")")"
cd "${FLATPAK_DIR}"

LIBCHEWING_URL="https://github.com/chewing/libchewing.git"
LIBCHEWING_TAG="v0.10.3"
LIBCHEWING_COMMIT="93a9a24bae6173f23c620df314b55c596c7622dd"

if [ ! -d flatpak-builder-tools ]; then
        git clone ${GIT_CLONE_ARGS} https://github.com/flatpak/flatpak-builder-tools
else
        git -C flatpak-builder-tools pull
fi

if [ ! -d libchewing ]; then
        git clone ${GIT_CLONE_ARGS} --branch "${LIBCHEWING_TAG}" "${LIBCHEWING_URL}" libchewing
else
        git -C libchewing fetch ${GIT_FETCH_ARGS} origin "${LIBCHEWING_TAG}"
fi

git -C libchewing -c advice.detachedHead=false checkout "${LIBCHEWING_COMMIT}"

./flatpak-builder-tools/cargo/flatpak-cargo-generator.py -o libchewing-cargo-sources.json libchewing/Cargo.lock
