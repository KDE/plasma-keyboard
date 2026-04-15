/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "inputmethodhints.h"

Qt::InputMethodHints mapInputMethodHints(InputPlugin::ContentHint contentHints, InputPlugin::ContentPurpose contentPurpose, bool autoCapitalizationEnabled)
{
    Qt::InputMethodHints hints;

    if ((contentHints & InputPlugin::content_hint_auto_completion) == 0 || (contentHints & InputPlugin::content_hint_auto_correction) == 0) {
        hints |= Qt::ImhNoPredictiveText;
    }
    if ((contentHints & InputPlugin::content_hint_auto_capitalization) == 0 || !autoCapitalizationEnabled) {
        hints |= Qt::ImhNoAutoUppercase;
    }
    if (contentHints & InputPlugin::content_hint_lowercase) {
        hints |= Qt::ImhPreferLowercase;
    }
    if (contentHints & InputPlugin::content_hint_uppercase) {
        hints |= Qt::ImhPreferUppercase;
    }
    if (contentHints & InputPlugin::content_hint_hidden_text) {
        hints |= Qt::ImhHiddenText;
        hints |= Qt::ImhSensitiveData;
    }
    if (contentHints & InputPlugin::content_hint_sensitive_data) {
        hints |= Qt::ImhSensitiveData;
    }
    if (contentHints & InputPlugin::content_hint_latin) {
        hints |= Qt::ImhPreferLatin;
    }
    if (contentHints & InputPlugin::content_hint_multiline) {
        hints |= Qt::ImhMultiLine;
    }

    switch (contentPurpose) {
    case InputPlugin::content_purpose_digits:
        hints |= Qt::ImhDigitsOnly;
        break;
    case InputPlugin::content_purpose_number:
        hints |= Qt::ImhPreferNumbers;
        break;
    case InputPlugin::content_purpose_phone:
        hints |= Qt::ImhDialableCharactersOnly;
        break;
    case InputPlugin::content_purpose_url:
        hints |= Qt::ImhUrlCharactersOnly;
        break;
    case InputPlugin::content_purpose_email:
        hints |= Qt::ImhEmailCharactersOnly;
        break;
    case InputPlugin::content_purpose_password:
        hints |= Qt::ImhSensitiveData;
        break;
    case InputPlugin::content_purpose_date:
        hints |= Qt::ImhDate;
        break;
    case InputPlugin::content_purpose_time:
        hints |= Qt::ImhTime;
        break;
    case InputPlugin::content_purpose_datetime:
        hints |= Qt::ImhDate | Qt::ImhTime;
        break;
    case InputPlugin::content_purpose_terminal:
        hints |= Qt::ImhPreferLatin;
        break;
    case InputPlugin::content_purpose_normal:
    case InputPlugin::content_purpose_alpha:
    case InputPlugin::content_purpose_name:
        break;
    }

    return hints;
}
