/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QPointer>
#include <QStringList>
#include <qqmlintegration.h>

class InputEngine;

class AbstractTextComposer : public QObject
{
    Q_OBJECT

public:
    enum class TextCase {
        Lower,
        Upper,
    };
    Q_ENUM(TextCase)

    explicit AbstractTextComposer(QObject *parent = nullptr);
    ~AbstractTextComposer() override;

    InputEngine *inputEngine() const;

    virtual bool setTextCase(TextCase textCase) = 0;
    virtual bool keyEvent(Qt::Key key, const QString &text) = 0;
    virtual bool selectCandidate(int index);
    Q_INVOKABLE virtual bool replaceLastInput(const QString &text);
    virtual bool showsPreeditBubble() const;

public Q_SLOTS:
    virtual void reset();
    virtual void update();

protected:
    QString preeditText() const;
    QString preeditPrefix() const;
    QStringList candidates() const;
    void setPreeditText(const QString &text);
    void setPreeditPrefix(const QString &text);
    void setCandidates(const QStringList &candidates);
    void clearComposition();
    void setInputEngine(InputEngine *inputEngine);

private:
    friend class InputEngine;
    QPointer<InputEngine> m_inputEngine;
};
