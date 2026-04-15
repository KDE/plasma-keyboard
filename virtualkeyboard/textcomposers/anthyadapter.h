/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QStringList>

struct anthy_input_context;
struct anthy_input_config;
struct anthy_context;

/**
 * Communication and state for Anthy support
 */
class AnthyAdapter
{
public:
    AnthyAdapter();
    ~AnthyAdapter();
    AnthyAdapter(const AnthyAdapter &) = delete;
    AnthyAdapter &operator=(const AnthyAdapter &) = delete;

    bool initialize();
    void setInputMode(const QString &inputMode);
    bool appendText(const QString &text);
    bool removeLastInput();
    bool replaceLastInput(const QString &text);
    bool cycleKanaModifier();
    void cycleCandidate();
    bool selectCandidate(int index);
    QString takeCommitText();
    void reset();

    bool hasComposition() const;
    QString preeditText() const;
    QStringList candidates() const;

private:
    static bool acceptsText(const QString &text);
    void applyInputMode();
    void rebuildContext();
    void appendCharacter(QChar character);
    void updateCandidates();

    QString m_inputMode = QStringLiteral("hiragana");
    QStringList m_inputChunks;
    QString m_convertedText;
    QString m_pendingText;
    QString m_selectedCandidate;
    QStringList m_candidates;
    anthy_input_context *m_context = nullptr;
    anthy_input_config *m_config = nullptr;
    anthy_context *m_conversionContext = nullptr;
};
