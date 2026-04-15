/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QList>
#include <QStringList>

#include <memory>

struct anthy_context;
struct anthy_input_config;
struct anthy_input_context;

/**
 * Communication and state for Anthy support
 *
 * Three steps:
 * 1. Key presses add romaji (ex. "ni", "ho", "n", "go")
 * 2. The romaji is spelled out as kana, called the reading (ex. にほんご)
 * 3. The reading is converted to one of anthy's candidates (ex. 日本語)
 */
class AnthyAdapter
{
public:
    AnthyAdapter();
    ~AnthyAdapter();
    AnthyAdapter(const AnthyAdapter &) = delete;
    AnthyAdapter &operator=(const AnthyAdapter &) = delete;

    // Whether text can be typed into the reading (romaji letters and some punctuation)
    static bool acceptsText(const QString &text);

    bool initialize();
    void setKatakana(bool katakana);

    // Step 1: Typing
    bool appendText(const QString &text);
    bool removeLastInput();
    bool replaceLastInput(const QString &text);
    bool cycleKanaModifier();

    bool isConverting() const;
    bool selectNextCandidate();
    bool selectCandidate(int index);
    void cancelConversion();

    QString takeCommitText();
    void reset();

    bool hasComposition() const;
    QString preeditText() const;
    QStringList candidates() const;

private:
    // Preedit state - kana + romaji (building)
    struct Reading {
        QString kana; // The kana so far
        QString pendingRomaji; // Trailing romaji that isn't a kana yet (ex. "ky")
    };

    struct AnthyDeleter {
        void operator()(anthy_context *context) const;
        void operator()(anthy_input_context *context) const;
        void operator()(anthy_input_config *config) const;
    };

    // Step 2: spelling the romaji as kana
    void updateComposition();
    Reading spellOut(const QString &romaji) const;
    QString readingForConversion() const;
    QString leftoverRomaji() const;
    QString toInputScript(const QString &hiragana) const;

    // Step 3: converting the reading
    void updateCandidates();
    QStringList conversionCandidates(const QString &reading) const;

    bool m_katakana = false;

    QStringList m_keyPresses;
    Reading m_reading;

    QStringList m_candidates;
    int m_selectedCandidate = -1;

    std::unique_ptr<anthy_input_config, AnthyDeleter> m_inputConfig;
    std::unique_ptr<anthy_input_context, AnthyDeleter> m_inputContext;
    std::unique_ptr<anthy_input_context, AnthyDeleter> m_probeContext;
    std::unique_ptr<anthy_context, AnthyDeleter> m_conversionContext;
};
