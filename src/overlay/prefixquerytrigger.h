/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "overlaytrigger.h"

/**
 * Trigger that activates an emoji picker when a prefix pattern is typed.
 *
 * Detects patterns like ":whale" or ":smile" in committed/preedit text
 * and opens an emoji picker filtered by the query after the colon.
 *
 * This is a stub implementation; full emoji search will be added later.
 */
class PrefixQueryTrigger : public OverlayTrigger
{
    Q_OBJECT

public:
    explicit PrefixQueryTrigger(QObject *parent = nullptr);
    ~PrefixQueryTrigger() override = default;

    QString triggerId() const override;
    QString displayName() const override;

    // clang-format off
    OverlayTriggerResult processEvent(OverlayInputEvent eventType,
                                            const QKeyEvent *keyEvent,
                                            const QString &text,
                                            OverlayController *controller) override;
    // clang-format on

    void reset() override;
    bool isEnabled() const override;
    QStringList candidates(const QString &baseText) const override;

    /**
     * Set the prefix character that starts emoji search (default: ':').
     */
    void setPrefix(QChar prefix);
    QChar prefix() const;

    /**
     * Set the minimum query length before showing suggestions.
     */
    void setMinQueryLength(int length);
    int minQueryLength() const;

private:
    /**
     * Extract query from text (characters after the prefix).
     */
    QString extractQuery(const QString &text) const;

    QChar m_prefix = u':';
    int m_minQueryLength = 2;
    QString m_currentQuery;
    bool m_active = false;
};
