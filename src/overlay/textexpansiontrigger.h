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
 * Detects configured trigger phrases in committed text and replaces them
 * with expansion text. Optionally requires a trigger character (e.g., Tab)
 * to confirm expansion.
 *
 * This is a stub implementation; snippet storage will be added later.
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

    /**
     * Add an expansion rule.
     *
     * @param abbreviation The trigger phrase (e.g., "brb").
     * @param expansion The text to insert (e.g., "be right back").
     */
    void addExpansion(const QString &abbreviation, const QString &expansion);

    /**
     * Remove an expansion rule.
     */
    void removeExpansion(const QString &abbreviation);

    /**
     * Set whether a trigger key is required to confirm expansion.
     */
    void setRequiresTriggerKey(bool value);
    bool requiresTriggerKey() const;

    /**
     * Set the trigger key (default: Tab).
     */
    void setTriggerKey(Qt::Key key);
    Qt::Key triggerKey() const;

private:
    /**
     * Check if the text ends with any known abbreviation.
     */
    QString findMatchingAbbreviation(const QString &text) const;

    QHash<QString, QString> m_expansions;
    bool m_requiresTriggerKey = true;
    Qt::Key m_triggerKey = Qt::Key_Tab;
    QString m_pendingAbbreviation;
};
