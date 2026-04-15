/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "abstracttextcomposer.h"

#include <qqmlintegration.h>

class DirectTextComposer : public AbstractTextComposer
{
    Q_OBJECT
    QML_NAMED_ELEMENT(DirectTextComposer)

public:
    explicit DirectTextComposer(QObject *parent = nullptr);
    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;

private:
    QString transformedText(const QString &text) const;

    TextCase m_textCase = TextCase::Lower;
};
