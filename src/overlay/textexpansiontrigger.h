/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "overlaytrigger.h"

#include <QHash>

/**
 * Trigger that expands typed abbreviations into full text.
 *
 * Detects configured abbreviations before the cursor and immediately replaces
 * them with expansion text.
 *
 */
class TextExpansionTrigger : public OverlayTrigger
{
    Q_OBJECT

public:
    explicit TextExpansionTrigger(QObject *parent = nullptr);
    ~TextExpansionTrigger() override = default;

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

private:
    /**
     * Check if the text ends with any known abbreviation.
     */
    QString findMatchingAbbreviation(const QString &text);

    QHash<QString, QString> m_expansions;
};
