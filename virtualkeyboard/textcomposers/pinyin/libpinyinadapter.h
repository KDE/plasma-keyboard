/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QStringList>

typedef struct _pinyin_instance_t pinyin_instance_t;
typedef struct _lookup_candidate_t lookup_candidate_t;

/**
 * Communication and state for libpinyin support
 */
class LibPinyinAdapter
{
public:
    LibPinyinAdapter() = default;
    ~LibPinyinAdapter();
    LibPinyinAdapter(const LibPinyinAdapter &) = delete;
    LibPinyinAdapter &operator=(const LibPinyinAdapter &) = delete;

    bool appendText(const QString &text);
    bool removeLastInput();
    QString takeCommitText(int index = 0);
    QString takeSurface();
    void reset();

    bool hasComposition() const;
    QString preeditText() const;
    QStringList candidates() const;

private:
    static bool acceptsText(const QString &text);
    bool rebuildLookup();
    void refreshLookup();

    pinyin_instance_t *m_instance = nullptr;

    // Null entries represent candidates that commit the entire surface
    QList<lookup_candidate_t *> m_lookupCandidates;
    QString m_surface; // the raw text being composed (e.g. "nihao")
    QStringList m_candidates;
};
