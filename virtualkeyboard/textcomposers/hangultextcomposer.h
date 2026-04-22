/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "abstracttextcomposer.h"

#include <QStringList>
#include <qqmlintegration.h>

typedef struct _HangulInputContext HangulInputContext;

class HangulTextComposer : public AbstractTextComposer
{
    Q_OBJECT
    Q_PROPERTY(QString inputMode READ inputMode WRITE setInputMode NOTIFY inputModeChanged)
    QML_NAMED_ELEMENT(HangulTextComposer)

public:
    explicit HangulTextComposer(QObject *parent = nullptr);
    ~HangulTextComposer() override;

    QString inputMode() const;
    void setInputMode(const QString &inputMode);

    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;
    void reset() override;
    void update() override;

Q_SIGNALS:
    void inputModeChanged();

private:
    void ensureContext();
    void updatePreedit();
    bool processHangulKey(int ascii, Qt::Key fallbackKey, const QString &fallbackText);
    bool flushComposition();
    QString transformedText(const QString &text) const;

    HangulInputContext *m_context = nullptr;
    QString m_inputMode = QStringLiteral("hangul");
    TextCase m_textCase = TextCase::Lower;
};
