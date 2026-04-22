/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "overlaycontroller.h"
#include "overlaytrigger.h"

#include <QHash>
#include <QStringList>
#include <QTimer>

class InputEngine;

/**
 * Trigger that activates an overlay after a long-press on a key.
 *
 * Used for diacritics and symbol alternate selection: hold a key (e.g., "a"
 * or "$") to show alternate characters (á, à, â, … or €, £, ¥, …).
 */
class LongPressTrigger : public OverlayTrigger
{
    Q_OBJECT

public:
    explicit LongPressTrigger(InputEngine *inputEngine, QObject *parent = nullptr);
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

    /**
     * Reload the diacritics map from data files using the currently enabled
     * locales from PlasmaKeyboardSettings.
     *
     * Call this when the user changes the enabled locale list at runtime so
     * the updated candidates take effect without restarting the keyboard.
     */
    void reloadMap();

private:
    /**
     * Checks if the key event should be considered for long-press diacritics.
     */
    bool shouldHandleKey(const QKeyEvent *event) const;
    QStringList currentLocales() const;

    InputEngine *m_inputEngine = nullptr;

    /** Map of base characters to their diacritic variants. */
    QHash<QChar, QStringList> m_diacriticsMap;

    int m_holdThresholdMs = 500;
    bool m_timerStarted = false;
};
