/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "abstracttextcomposer.h"

#include <QStringList>
#include <QtCore/qstringliteral.h>
#include <qqmlintegration.h>

typedef struct anthy_input_context anthy_input_context;
typedef struct anthy_input_config anthy_input_config;

using namespace Qt::StringLiterals;

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
    bool setTextCase(TextCase textCase) override;
    bool keyEvent(Qt::Key key, const QString &text) override;
    bool selectCandidate(int index) override;
    bool replaceLastInput(const QString &text) override;
    bool showsPreeditBubble() const override;
    void reset() override;
    void update() override;

private:
    void ensureContext();
    void applyInputMode(const QString &inputMode);
    void rebuildContext();
    void refreshState();
    QStringList previewCandidates() const;
    bool commitComposition();
    bool hasComposition() const;
    bool applyDakutenMark(const QString &text);
    bool isRomajiInput(const QString &text) const;
    QString anthyChunkForText(const QString &text) const;

    QStringList m_inputChunks;
    QString m_selectedCandidate;
    QString m_inputMode = u"hiragana"_s;
    anthy_input_context *m_context = nullptr;
    anthy_input_config *m_config = nullptr;

Q_SIGNALS:
    void inputModeChanged();
};
