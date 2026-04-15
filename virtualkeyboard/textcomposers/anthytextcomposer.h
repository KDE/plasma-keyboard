/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "abstracttextcomposer.h"
#include "anthyadapter.h"

#include <QStringList>
#include <qqmlintegration.h>

class AnthyTextComposer : public AbstractTextComposer
{
    Q_OBJECT
    Q_PROPERTY(QString inputMode READ inputMode WRITE setInputMode NOTIFY inputModeChanged)
    QML_NAMED_ELEMENT(AnthyTextComposer)

public:
    explicit AnthyTextComposer(QObject *parent = nullptr);
    ~AnthyTextComposer() override;

    QString inputMode() const;
    void setInputMode(const QString &inputMode);
    Q_INVOKABLE bool cycleKanaModifier();
    bool autoCapitalizationSupported() const override;
    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;
    bool selectCandidate(int index) override;
    bool replaceLastInput(const QString &text) override;
    bool showsPreeditBubble() const override;
    void reset() override;

private:
    void refreshState();
    bool commitComposition();
    QString applyCapitalization(const QString &text) const;

    AnthyAdapter m_anthy;
    QString m_inputMode = QStringLiteral("hiragana");
    TextCase m_textCase = TextCase::Lower;

Q_SIGNALS:
    void inputModeChanged();
};
