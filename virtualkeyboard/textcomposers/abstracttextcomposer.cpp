/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "abstracttextcomposer.h"

#include "../inputengine.h"

AbstractTextComposer::AbstractTextComposer(QObject *parent)
    : QObject(parent)
{
}

AbstractTextComposer::~AbstractTextComposer() = default;

InputEngine *AbstractTextComposer::inputEngine() const
{
    return m_inputEngine;
}

bool AbstractTextComposer::selectCandidate(int index)
{
    Q_UNUSED(index)
    return false;
}

bool AbstractTextComposer::replaceLastInput(const QString &text)
{
    Q_UNUSED(text)
    return false;
}

bool AbstractTextComposer::showsPreeditBubble() const
{
    return false;
}

void AbstractTextComposer::reset()
{
    clearComposition();
}

void AbstractTextComposer::update()
{
}

QString AbstractTextComposer::preeditText() const
{
    return inputEngine() ? inputEngine()->preeditText() : QString();
}

QString AbstractTextComposer::preeditPrefix() const
{
    return inputEngine() ? inputEngine()->preeditPrefix() : QString();
}

QStringList AbstractTextComposer::candidates() const
{
    return inputEngine() ? inputEngine()->candidates() : QStringList();
}

void AbstractTextComposer::setPreeditText(const QString &text)
{
    if (auto *engine = inputEngine()) {
        engine->setPreeditText(text);
    }
}

void AbstractTextComposer::setPreeditPrefix(const QString &text)
{
    if (auto *engine = inputEngine()) {
        engine->setPreeditPrefix(text);
    }
}

void AbstractTextComposer::setCandidates(const QStringList &candidates)
{
    if (auto *engine = inputEngine()) {
        engine->setCandidates(candidates);
    }
}

void AbstractTextComposer::clearComposition()
{
    setPreeditPrefix(QString());
    setPreeditText(QString());
    setCandidates({});
}

void AbstractTextComposer::setInputEngine(InputEngine *inputEngine)
{
    m_inputEngine = inputEngine;
}
