/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QKeyEvent>
#include <QObject>
#include <QString>
#include <QStringList>

class OverlayController;

/**
 * Input event types that triggers can consume.
 *
 * Each type corresponds to a different input path and timing.
 */
enum class OverlayInputEvent {
    /** Physical/virtual key pressed. */
    KeyPress,
    /** Physical/virtual key released. */
    KeyRelease,
    /** Preedit (composing) text changed. */
    PreeditChanged,
    /** Text was committed to the input field. */
    TextCommitted,
    /** Timer expired (e.g., long-press hold). */
    TimerExpired,
};

/**
 * Actions a trigger can request from the controller.
 */
enum class OverlayAction {
    /** No action needed. */
    None,
    /** Open the overlay popup with candidates. */
    OpenOverlay,
    /** Close the overlay without committing. */
    CloseOverlay,
    /** Commit the selected/pending text. */
    CommitText,
    /** Replace surrounding text (for text expansion). */
    ReplaceText,
    /** Consume the event but take no visible action. */
    ConsumeEvent,
    /** Start a timer for delayed overlay activation (e.g., long-press). */
    StartTimer,
};

/**
 * Result returned by a trigger after processing an event.
 */
struct OverlayTriggerResult {
    /** The action the controller should take. */
    OverlayAction action = OverlayAction::None;

    /** Whether the input event should be consumed (not passed through). */
    bool consumeEvent = false;

    /** Query string for filtering candidates (e.g., ":whale" -> "whale"). */
    QString query;

    /** Text to commit or insert. */
    QString commitText;

    /** For ReplaceText: number of characters before cursor to delete. */
    int deleteBeforeCursor = 0;

    /** For StartTimer: duration in milliseconds before overlay opens. */
    int timerDurationMs = 0;

    /** For StartTimer: the key code that started the timer (for release handling). */
    int pendingKey = 0;

    /** For StartTimer: the pending text to be committed or shown in overlay. */
    QString pendingText;
};

/**
 * Abstract interface for overlay trigger strategies.
 *
 * Each trigger implementation detects a specific input pattern (long-press, prefix query,
 * phrase expansion) and returns an action for the controller to execute.
 *
 * Triggers are stateless event processors; the controller manages shared state like
 * pending text and popup visibility.
 */
class OverlayTrigger : public QObject
{
    Q_OBJECT

public:
    explicit OverlayTrigger(QObject *parent = nullptr);
    ~OverlayTrigger() override = default;

    /**
     * Unique identifier for this trigger type.
     *
     * Used for logging, settings, and selecting the appropriate overlay view.
     */
    virtual QString triggerId() const = 0;

    /**
     * Human-readable name for settings UI.
     */
    virtual QString displayName() const = 0;

    // clang-format off
    /**
     * Process an input event and return the resulting action.
     *
     * @param eventType The type of input event.
     * @param keyEvent The key event (may be null for non-key events).
     * @param text Associated text (preedit, committed, or pending).
     * @param controller The controller, for querying state.
     * @return The action to take.
     */
    virtual OverlayTriggerResult processEvent(OverlayInputEvent eventType,
                                                    const QKeyEvent *keyEvent,
                                                    const QString &text,
                                                    OverlayController *controller) = 0;
    // clang-format on

    /**
     * Called when the overlay is cancelled or closed.
     *
     * Triggers should reset any internal state.
     */
    virtual void reset() = 0;

    /**
     * Whether this trigger is currently enabled.
     */
    virtual bool isEnabled() const = 0;

    /**
     * Candidate options for the current trigger state.
     *
     * @param baseText The base text that triggered the overlay.
     * @return List of candidate strings to display.
     */
    virtual QStringList candidates(const QString &baseText) const = 0;
};
