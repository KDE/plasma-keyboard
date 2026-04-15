/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "basictextcomposer.h"

BasicTextComposer::BasicTextComposer(QObject *parent)
    : DirectTextComposer(parent)
{
}

QString BasicTextComposer::sentenceEndingCharacters() const
{
    return m_sentenceEndingCharacters;
}

void BasicTextComposer::setSentenceEndingCharacters(const QString &characters)
{
    if (m_sentenceEndingCharacters == characters) {
        return;
    }

    m_sentenceEndingCharacters = characters;
    Q_EMIT sentenceEndingCharactersChanged();
    Q_EMIT autoCapitalizationPolicyChanged();
}

bool BasicTextComposer::autoCapitalizationSupported() const
{
    return true;
}

bool BasicTextComposer::shouldAutoCapitalize(QStringView textBeforeCursor) const
{
    if (textBeforeCursor.trimmed().isEmpty()) {
        return true;
    }
    if (textBeforeCursor.size() < 2 || !textBeforeCursor.endsWith(QLatin1Char(' '))) {
        return false;
    }

    return m_sentenceEndingCharacters.contains(textBeforeCursor.at(textBeforeCursor.size() - 2));
}
