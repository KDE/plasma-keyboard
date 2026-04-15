/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "../abstracttextcomposer.h"
#include "libpinyinadapter.h"

#include <QStringList>
#include <qqmlintegration.h>

class PinyinTextComposer : public AbstractTextComposer
{
    Q_OBJECT
    Q_PROPERTY(QString inputMode READ inputMode WRITE setInputMode NOTIFY inputModeChanged)
    QML_NAMED_ELEMENT(PinyinTextComposer)

public:
    explicit PinyinTextComposer(QObject *parent = nullptr);
    ~PinyinTextComposer() override;
    QString inputMode() const;
    void setInputMode(const QString &inputMode);
    bool autoCapitalizationSupported() const override;
    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;
    bool selectCandidate(int index) override;
    bool showsPreeditBubble() const override;
    void reset() override;

private:
    bool commitCurrentSelection(int index = 0);
    bool commitSurface();
    void refreshState();
    QString transformedText(const QString &text) const;

    LibPinyinAdapter m_pinyin;
    TextCase m_textCase = TextCase::Lower;
    QString m_inputMode = QStringLiteral("pinyin");

Q_SIGNALS:
    void inputModeChanged();
};
