/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QPointer>
#include <QStringList>
#include <QStringView>
#include <qqmlintegration.h>

class InputEngine;

/**
 * Base class that sits between the keyboard and input engine, and converts
 * keyboard input into composed text and word candidates.
 *
 * Extend it to implement language specific features (e.g. word prediction,
 * character transformation for Korean).
 *
 * @see DirectTextComposer
 */
class AbstractTextComposer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoCapitalizationSupported READ autoCapitalizationSupported NOTIFY autoCapitalizationSupportedChanged)

public:
    enum class TextCase {
        Lower,
        Upper,
    };
    Q_ENUM(TextCase)

    explicit AbstractTextComposer(QObject *parent = nullptr);
    ~AbstractTextComposer() override;

    InputEngine *inputEngine() const;

    virtual bool autoCapitalizationSupported() const = 0;
    virtual bool setTextCase(TextCase textCase) = 0;
    virtual bool shouldAutoCapitalize(QStringView textBeforeCursor) const;
    virtual bool keyEvent(Qt::Key key, const QString &text) = 0;
    virtual bool selectCandidate(int index);
    Q_INVOKABLE virtual bool replaceLastInput(const QString &text);
    virtual bool showsPreeditBubble() const;

public Q_SLOTS:
    virtual void reset();

Q_SIGNALS:
    void autoCapitalizationSupportedChanged();
    void autoCapitalizationPolicyChanged();

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
