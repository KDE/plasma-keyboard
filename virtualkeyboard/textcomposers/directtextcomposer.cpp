/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "directtextcomposer.h"

#include "../inputengine.h"

#include <QLocale>

DirectTextComposer::DirectTextComposer(QObject *parent)
    : AbstractTextComposer(parent)
{
}

bool DirectTextComposer::setTextCase(TextCase textCase)
{
    m_textCase = textCase;
    return true;
}

bool DirectTextComposer::keyEvent(Qt::Key key, const QString &text)
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    return engine->handleKeyCommit(key, transformedText(text));
}

QString DirectTextComposer::transformedText(const QString &text) const
{
    if (m_textCase != TextCase::Upper || text.size() != 1) {
        return text;
    }

    return QLocale(inputEngine() ? inputEngine()->locale() : QString()).toUpper(text);
}
