/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "overlaycontroller.h"
#include "overlaytrigger.h"

#include <QHash>
#include <QTimer>

/**
 * Trigger that activates an overlay after a long-press on a letter key.
 *
 * Used for diacritics selection: hold a letter key (e.g., "a") to show
 * accent variants (á, à, â, etc.).
 */
class LongPressTrigger : public OverlayTrigger
{
    Q_OBJECT

public:
    explicit LongPressTrigger(QObject *parent = nullptr);
    ~LongPressTrigger() override = default;

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
     * Set the hold threshold in milliseconds.
     *
     * @param ms Threshold before popup appears.
     */
    void setHoldThreshold(int ms);

    /**
     * Get the current hold threshold.
     */
    int holdThreshold() const;

private:
    /**
     * Checks if the key event should be considered for long-press diacritics.
     */
    bool shouldHandleKey(const QKeyEvent *event) const;

    /** Map of base characters to their diacritic variants. */
    QHash<QChar, QStringList> m_diacriticsMap;

    int m_holdThresholdMs = 500;
    bool m_timerStarted = false;
};
