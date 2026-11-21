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
     * Use the locales of the given physical keyboard layouts (XKB layout names).
     */
    void setKeyboardLayouts(const QStringList &layouts, uint activeLayout);

    /**
     * Reload the diacritics map using the keyboard layout locales, or the
     * system locale if none have diacritics data.
     */
    void reloadMap();

private:
    /**
     * Checks if the key event should be considered for long-press diacritics.
     */
    bool shouldHandleKey(const QKeyEvent *event) const;
    QStringList currentLocales() const;

    /** Diacritics locales for the physical keyboard layouts, active layout first. */
    QStringList m_layoutLocales;

    /** Map of base characters to their diacritic variants. */
    QHash<QChar, QStringList> m_diacriticsMap;

    int m_holdThresholdMs = 500;
    bool m_timerStarted = false;
};
