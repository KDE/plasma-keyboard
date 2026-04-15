/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QByteArray>
#include <QStringList>

struct ChewingContext;

/**
 * Communication and state for libchewing support
 */
class LibChewingAdapter
{
public:
    LibChewingAdapter() = default;
    ~LibChewingAdapter();
    LibChewingAdapter(const LibChewingAdapter &) = delete;
    LibChewingAdapter &operator=(const LibChewingAdapter &) = delete;

    bool initialize();
    bool appendText(const QString &text);
    bool removeLastInput();
    bool selectCandidate(int index);
    QString takeCommitText();
    QString takePreeditText();
    void reset();

    bool hasComposition() const;
    QString preeditText() const;
    QString preeditPrefix() const;
    QStringList candidates() const;

private:
    void refreshState();
    void collectPendingText();
    QStringList currentCandidates();
    QStringList previewCandidatesForCurrentStrokeBuffer();
    ChewingContext *createPreviewContext() const;
    int chewingLayoutId() const;
    int strokeForText(const QString &text) const;

    QByteArray m_strokeBuffer;
    QString m_preeditText;
    QString m_preeditPrefix;
    QStringList m_candidates;
    QString m_commitText;
    bool m_candidatesUsePreviewContext = false;
    ChewingContext *m_context = nullptr;
};
