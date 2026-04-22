/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "abstracttextcomposer.h"

#include <QStringList>
#include <qqmlintegration.h>

typedef struct _pinyin_instance_t pinyin_instance_t;
typedef struct _lookup_candidate_t lookup_candidate_t;

class PinyinTextComposer : public AbstractTextComposer
{
    Q_OBJECT
    Q_PROPERTY(QString inputMode READ inputMode WRITE setInputMode NOTIFY inputModeChanged)
    QML_NAMED_ELEMENT(PinyinTextComposer)

public:
    explicit PinyinTextComposer(QObject *parent = nullptr);
    QString inputMode() const;
    void setInputMode(const QString &inputMode);
    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;
    bool selectCandidate(int index) override;
    bool showsPreeditBubble() const override;
    void reset() override;
    void update() override;

private:
    bool commitCurrentSelection(const QString &text = QString());
    bool commitSurface();
    bool isPinyinInput(const QString &text) const;
    bool rebuildLookup();
    void refreshLookup();

    TextCase m_textCase = TextCase::Lower;
    QString m_inputMode = QStringLiteral("pinyin");
    QString m_surface;

Q_SIGNALS:
    void inputModeChanged();
};
