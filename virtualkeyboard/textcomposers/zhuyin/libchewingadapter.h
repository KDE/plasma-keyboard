/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QStringList>

#include <memory>

struct ChewingContext;

/**
 * Communication and state for libchewing support
 */
class LibChewingAdapter
{
public:
    bool initialize();
    bool appendText(const QString &text);
    bool applyFirstTone();
    bool removeLastInput();
    bool selectCandidate(int index);
    QString takeCommitText();
    QString takePreeditText();
    void reset();

    bool hasComposition() const;
    bool hasPendingBopomofo() const;
    QString preeditText() const;
    QString preeditPrefix() const;
    QStringList candidates() const;

private:
    void refreshState();
    void collectPendingText();
    void updateCandidates();
    void collectCandidates(ChewingContext *context);
    bool updatePreviewCandidates();

    struct ContextDeleter {
        void operator()(ChewingContext *context) const;
    };
    using ContextPtr = std::unique_ptr<ChewingContext, ContextDeleter>;

    QString m_preeditText;
    QString m_preeditPrefix;
    QStringList m_candidates;
    QString m_commitText;

    bool m_candidatesUsePreviewContext = false;

    ContextPtr m_context;
    ContextPtr m_previewContext;
};
