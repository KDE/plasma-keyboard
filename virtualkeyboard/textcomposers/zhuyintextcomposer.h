/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "abstracttextcomposer.h"

#include <QStringList>
#include <qqmlintegration.h>

typedef struct ChewingContext ChewingContext;

class ZhuyinTextComposer : public AbstractTextComposer
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ZhuyinTextComposer)
    Q_PROPERTY(QString inputMode READ inputMode WRITE setInputMode NOTIFY inputModeChanged)

public:
    explicit ZhuyinTextComposer(QObject *parent = nullptr);
    ~ZhuyinTextComposer() override;

    QString inputMode() const;
    void setInputMode(const QString &inputMode);

    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;
    bool selectCandidate(int index) override;
    bool showsPreeditBubble() const override;
    void reset() override;
    void update() override;

Q_SIGNALS:
    void inputModeChanged();

private:
    void ensureContext();
    void refreshState();
    bool hasComposition() const;
    bool commitPendingText();
    bool commitPreeditBuffer();
    QStringList currentCandidates();
    QStringList previewCandidatesForCurrentStrokeBuffer() const;
    ChewingContext *createPreviewContext() const;
    int chewingLayoutId() const;
    int strokeForText(const QString &text) const;
    QString transformedText(const QString &text) const;

    QByteArray m_strokeBuffer;
    QString m_inputMode = QStringLiteral("zhuyin");
    TextCase m_textCase = TextCase::Lower;
    bool m_candidatesUsePreviewContext = false;
    ChewingContext *m_context = nullptr;
};
