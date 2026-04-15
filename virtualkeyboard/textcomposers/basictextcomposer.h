/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "directtextcomposer.h"

// Text composer that passes input directly to the backend, with added support for auto-capitalization (if enabled).

class BasicTextComposer : public DirectTextComposer
{
    Q_OBJECT
    QML_NAMED_ELEMENT(BasicTextComposer)
    Q_PROPERTY(QString sentenceEndingCharacters READ sentenceEndingCharacters WRITE setSentenceEndingCharacters NOTIFY sentenceEndingCharactersChanged)

public:
    explicit BasicTextComposer(QObject *parent = nullptr);

    QString sentenceEndingCharacters() const;
    void setSentenceEndingCharacters(const QString &characters);

    bool autoCapitalizationSupported() const override;
    bool shouldAutoCapitalize(QStringView textBeforeCursor) const override;

Q_SIGNALS:
    void sentenceEndingCharactersChanged();

private:
    QString m_sentenceEndingCharacters = QStringLiteral(".!?¡¿");
};
