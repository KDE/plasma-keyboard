/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "inputplugin.h"

#include <Qt>

Qt::InputMethodHints mapInputMethodHints(InputPlugin::ContentHint contentHints, InputPlugin::ContentPurpose contentPurpose, bool autoCapitalizationEnabled);
