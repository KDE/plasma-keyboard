/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "../abstracttextcomposer.h"
#include "libchewingadapter.h"

#include <QStringList>
#include <qqmlintegration.h>

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

    bool autoCapitalizationSupported() const override;
    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;
    bool selectCandidate(int index) override;
    bool showsPreeditBubble() const override;
    void reset() override;

Q_SIGNALS:
    void inputModeChanged();

private:
    void refreshState();
    bool commitPendingText();
    bool commitPreeditBuffer();
    QString transformedText(const QString &text) const;

    LibChewingAdapter m_chewing;
    QString m_inputMode = QStringLiteral("zhuyin");
    TextCase m_textCase = TextCase::Lower;
};
