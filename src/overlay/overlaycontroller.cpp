/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "overlaycontroller.h"

#include "inputplugin.h"
#include "logging.h"
#include "overlaytrigger.h"

OverlayController::OverlayController(InputPlugin *inputPlugin, QObject *parent)
    : QObject(parent)
    , m_inputPlugin(inputPlugin)
    , m_candidateModel(new CandidateModel(this))
{
    m_holdTimer.setSingleShot(true);
    connect(&m_holdTimer, &QTimer::timeout, this, &OverlayController::handleTimerExpired);

    // The settle timer is a single-shot guard that absorbs extra surrounding_text
    // echoes from clients that send more than one event per commit_string.
    // 100 ms is well above any realistic Wayland roundtrip but well below the
    // long-press threshold, so genuine external cursor moves are not masked.
    static constexpr int SURROUNDING_TEXT_SETTLE_DELAY_MS = 100;
    m_surroundingTextSettleTimer.setSingleShot(true);
    m_surroundingTextSettleTimer.setInterval(SURROUNDING_TEXT_SETTLE_DELAY_MS);
}

OverlayController::~OverlayController()
{
}

void OverlayController::registerTrigger(OverlayTrigger *trigger)
{
    if (!trigger) {
        return;
    }
    trigger->setParent(this);
    m_triggers.append(trigger);
}

bool OverlayController::processKeyPress(QKeyEvent *event)
{
    if (!event) {
        return false;
    }

    // If overlay is visible, handle Esc to cancel
    if (m_overlayVisible) {
        if (event->key() == Qt::Key_Escape) {
            qCDebug(PlasmaKeyboard) << "Esc pressed while overlay open; cancelling";
            cancelOverlay();
            return true;
        }

        // Handle number keys 1-9 for candidate selection
        if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_9) {
            int index = event->key() - Qt::Key_1;
            if (index < m_candidateModel->rowCount()) {
                qCDebug(PlasmaKeyboard) << "Number key" << (index + 1) << "pressed; committing candidate" << index;
                commitCandidate(index);
                return true;
            }
            // Number beyond available candidates - fall through to cancel
        }

        // Ignore repeats of the pending key
        if (event->key() == m_pendingKey) {
            return true;
        }

        // Any other key cancels the overlay
        cancelOverlay();
        // Fall through to process the new key
    }

    // Suppress auto-repeat key events for the pending key, so long-press for diacritics
    // works without accidentally triggering repeat input on the client side.
    //
    // Wayland key events always have isAutoRepeat() == false because QKeyEvent is
    // constructed manually, so we detect repeats by checking whether the same key is
    // already pending.
    if (!m_pendingText.isEmpty() && event->key() == m_pendingKey) {
        qCDebug(PlasmaKeyboard) << "Suppressing repeat press of pending overlay key" << m_pendingKey;
        return true;
    }

    // Dispatch to triggers in order
    for (auto *trigger : m_triggers) {
        if (!trigger->isEnabled()) {
            continue;
        }

        auto result = trigger->processEvent(OverlayInputEvent::KeyPress, event, event->text(), this);
        if (result.action != OverlayAction::None || result.consumeEvent) {
            executeAction(result, trigger);
            return result.consumeEvent;
        }
    }

    return false;
}

bool OverlayController::processKeyRelease(QKeyEvent *event)
{
    if (!event) {
        return false;
    }

    // Swallow release of a discarded/committed pending key
    if (m_swallowNextRelease && event->key() == m_ignoreReleaseKey) {
        if (!event->isAutoRepeat()) {
            qCDebug(PlasmaKeyboard) << "Swallowing release for discarded overlay key" << m_ignoreReleaseKey;
            m_swallowNextRelease = false;
            m_ignoreReleaseKey = 0;
        }
        return true;
    }

    // Handle pending key release
    if (!m_pendingText.isEmpty() && event->key() == m_pendingKey) {
        // Stop timer if still active
        if (m_holdTimer.isActive()) {
            m_holdTimer.stop();
        }

        if (m_overlayVisible) {
            // Release doesn't close overlay; wait for selection or cancel
            m_pendingKeyReleased = true;
            return true;
        }

        // No overlay yet — the base character was already committed via
        // commit_string on press, so nothing to commit here. Just clear
        // pending state and consume the release (the raw press was consumed,
        // so there's no matching press on the client side).
        qCDebug(PlasmaKeyboard) << "Releasing before overlay; base character already committed on press";
        resetState();
        return true;
    }

    // Dispatch to triggers for release handling
    for (auto *trigger : m_triggers) {
        if (!trigger->isEnabled()) {
            continue;
        }

        auto result = trigger->processEvent(OverlayInputEvent::KeyRelease, event, event->text(), this);
        if (result.action != OverlayAction::None || result.consumeEvent) {
            executeAction(result, trigger);
            return result.consumeEvent;
        }
    }

    return false;
}

bool OverlayController::processPreeditChanged(const QString &preedit)
{
    for (auto *trigger : m_triggers) {
        if (!trigger->isEnabled()) {
            continue;
        }

        auto result = trigger->processEvent(OverlayInputEvent::PreeditChanged, nullptr, preedit, this);
        if (result.action != OverlayAction::None) {
            executeAction(result, trigger);
            return true;
        }
    }
    return false;
}

bool OverlayController::processTextCommitted(const QString &text)
{
    for (auto *trigger : m_triggers) {
        if (!trigger->isEnabled()) {
            continue;
        }

        auto result = trigger->processEvent(OverlayInputEvent::TextCommitted, nullptr, text, this);
        if (result.action != OverlayAction::None) {
            executeAction(result, trigger);
            return true;
        }
    }
    return false;
}

bool OverlayController::overlayVisible() const
{
    return m_overlayVisible;
}

QString OverlayController::activeTriggerId() const
{
    return m_activeTriggerId;
}

QString OverlayController::pendingText() const
{
    return m_pendingText;
}

CandidateModel *OverlayController::candidateModel() const
{
    return m_candidateModel;
}

int OverlayController::pendingKey() const
{
    return m_pendingKey;
}

InputPlugin *OverlayController::inputPlugin() const
{
    return m_inputPlugin;
}

void OverlayController::commitCandidate(int index)
{
    const QString text = m_candidateModel->insertTextAt(index);
    if (text.isEmpty()) {
        return;
    }
    commitText(text);
}

void OverlayController::commitText(const QString &text)
{
    if (text.isEmpty()) {
        return;
    }

    qCDebug(PlasmaKeyboard) << "Committing overlay selection:" << text;

    if (m_inputPlugin) {
        // The base character (m_pendingText) is still present in the text field —
        // it was committed on key-press and never retracted. Delete it now and
        // replace it with the selected candidate in one atomic operation:
        //   delete_surrounding_text is deferred by the compositor until the
        //   following commit_string, so sending both requests back-to-back
        //   guarantees they are applied together at the current cursor position.
        if (!m_pendingText.isEmpty()) {
            const int charCount = m_pendingText.length();
            m_inputPlugin->deleteSurroundingText(-charCount, charCount);
        }
        m_inputPlugin->commit(text);
    }

    if (m_overlayVisible) {
        Q_EMIT overlayClosed();
    }

    // Only swallow the next release if the key is still physically held
    if (!m_pendingKeyReleased) {
        m_ignoreReleaseKey = m_pendingKey;
        m_swallowNextRelease = true;
    }

    resetState();
}

void OverlayController::cancelOverlay()
{
    // The base character was committed on key-press and is already present in
    // the text field. It was never retracted, so there is nothing to restore
    // regardless of whether the overlay was visible or only the hold timer was
    // running. Simply close the overlay and reset state.

    if (m_overlayVisible) {
        Q_EMIT overlayClosed();
    }

    if (!m_pendingText.isEmpty() && !m_pendingKeyReleased) {
        m_ignoreReleaseKey = m_pendingKey;
        m_swallowNextRelease = true;
    }

    // Reset all triggers
    for (auto *trigger : m_triggers) {
        trigger->reset();
    }

    resetState();
}

void OverlayController::handleSurroundingTextChanged()
{
    // If we are expecting a surrounding-text update from one of our own
    // commit/delete-surrounding-text operations, consume that credit and return.
    if (m_pendingSurroundingTextUpdates > 0) {
        qCDebug(PlasmaKeyboard) << "Ignoring self-caused surrounding-text update (" << m_pendingSurroundingTextUpdates << " remaining)";
        --m_pendingSurroundingTextUpdates;
        return;
    }

    // Counter exhausted — fall back to the settle timer.
    //
    // Some client apps (Firefox, Chromium, VS Code, …) send two or more
    // surrounding_text events for a single commit_string. The credit counter
    // above covers the first echo; the settle timer absorbs any remaining
    // echoes without a cursor-position comparison (which is unreliable when
    // the client reports a shifted surrounding-text window after insertion).
    if (m_surroundingTextSettleTimer.isActive()) {
        qCDebug(PlasmaKeyboard) << "Ignoring extra surrounding-text echo (within settle window)";
        return;
    }

    // An external event changed the cursor (e.g. user tapped elsewhere in the
    // text field). Cancel any pending overlay state.
    if (m_holdTimer.isActive() || m_overlayVisible) {
        qCDebug(PlasmaKeyboard) << "External cursor move detected while overlay active; cancelling overlay";
        cancelOverlay();
    }
}

void OverlayController::openOverlay(const QString &triggerId, const QString &baseText, const QStringList &candidates)
{
    if (candidates.isEmpty()) {
        // No candidates: the base character was already committed on key-press
        // and is present in the text field, so no further commit is needed.
        // Simply reset state and leave the text as-is.
        resetState();
        return;
    }

    m_overlayVisible = true;
    m_activeTriggerId = triggerId;
    m_pendingText = baseText;

    m_candidateModel->setTriggerId(triggerId);
    m_candidateModel->setCandidates(candidates);

    Q_EMIT overlayVisibleChanged();
    Q_EMIT activeTriggerIdChanged();
    Q_EMIT pendingTextChanged();
    Q_EMIT overlayRequested(triggerId, baseText);
}

void OverlayController::handleTimerExpired()
{
    if (m_pendingText.isEmpty() || !m_pendingTrigger) {
        return;
    }

    auto result = m_pendingTrigger->processEvent(OverlayInputEvent::TimerExpired, nullptr, m_pendingText, this);

    // If opening the overlay, the base character that was committed on key-press
    // is intentionally left in the text field. It will be replaced atomically
    // (delete + commit_string) when the user selects a candidate in commitText().
    // Do NOT call deleteSurroundingText here: delete_surrounding_text is deferred
    // by the compositor until the next commit_string, so a standalone delete
    // without a following commit would be consumed by a future unrelated commit
    // at a potentially different cursor position, corrupting the text.
    executeAction(result, m_pendingTrigger);
}

void OverlayController::executeAction(const OverlayTriggerResult &result, OverlayTrigger *trigger)
{
    switch (result.action) {
    case OverlayAction::OpenOverlay: {
        const auto candidates = trigger->candidates(m_pendingText);
        openOverlay(trigger->triggerId(), m_pendingText, candidates);
        break;
    }
    case OverlayAction::CloseOverlay:
        cancelOverlay();
        break;
    case OverlayAction::CommitText:
        if (!result.commitText.isEmpty()) {
            commitText(result.commitText);
        } else if (!m_pendingText.isEmpty()) {
            commitText(m_pendingText);
        }
        break;
    case OverlayAction::ReplaceText:
        // For text expansion: delete chars before cursor, then commit.
        // Both operations are sent back-to-back; delete_surrounding_text is
        // deferred by the compositor until the following commit_string, so
        // they are applied atomically. No counter increment is needed —
        // resetState() is called immediately below before the compositor's
        // surrounding_text echo can arrive, so the echo is harmless.
        if (m_inputPlugin && result.deleteBeforeCursor > 0) {
            m_inputPlugin->deleteSurroundingText(-result.deleteBeforeCursor, result.deleteBeforeCursor);
        }
        if (!result.commitText.isEmpty() && m_inputPlugin) {
            m_inputPlugin->commit(result.commitText);
        }
        resetState();
        break;
    case OverlayAction::StartTimer:
        // Flush any previous pending state to prevent character drops during
        // key rollover. The old character was already committed via
        // commit_string on press, so no re-commit is needed — just mark its
        // release for swallowing (since the raw press was consumed, the client
        // never saw it, and a stray release would be unmatched).
        if (!m_pendingText.isEmpty()) {
            qCDebug(PlasmaKeyboard) << "New pending key arrived; flushing old pending state for" << m_pendingText;
            if (m_holdTimer.isActive()) {
                m_holdTimer.stop();
            }
            m_ignoreReleaseKey = m_pendingKey;
            m_swallowNextRelease = true;
        }

        // Set new pending state and start the hold timer
        m_pendingText = result.pendingText;
        m_pendingKey = result.pendingKey;
        m_pendingTrigger = trigger;
        if (result.timerDurationMs > 0) {
            qCDebug(PlasmaKeyboard) << "Starting timer for" << m_pendingText << "duration" << result.timerDurationMs << "ms";
            m_holdTimer.start(result.timerDurationMs);
        }

        // Immediately commit the base character via the input-method protocol
        // (commit_string). This gives zero-latency character appearance without
        // triggering client-side key repeat (unlike forwarding the raw key).
        if (m_inputPlugin && !m_pendingText.isEmpty()) {
            qCDebug(PlasmaKeyboard) << "Immediately committing base character" << m_pendingText;
            // The primary credit counter absorbs the first echo. The settle
            // timer absorbs any additional echoes from clients that send
            // multiple surrounding_text events per commit_string.
            ++m_pendingSurroundingTextUpdates;
            m_surroundingTextSettleTimer.start();
            m_inputPlugin->commit(m_pendingText);
        }
        break;
    case OverlayAction::ConsumeEvent:
        // Event consumed but no visible action
        break;
    case OverlayAction::None:
        break;
    }
}

void OverlayController::resetState()
{
    if (m_holdTimer.isActive()) {
        m_holdTimer.stop();
    }

    const bool wasVisible = m_overlayVisible;
    const bool hadPending = !m_pendingText.isEmpty();
    const bool hadTrigger = !m_activeTriggerId.isEmpty();

    m_overlayVisible = false;
    m_pendingText.clear();
    m_pendingKey = 0;
    m_pendingKeyReleased = false;
    m_activeTriggerId.clear();
    m_pendingTrigger = nullptr;
    m_candidateModel->clear();
    m_pendingSurroundingTextUpdates = 0;
    m_surroundingTextSettleTimer.stop();

    if (wasVisible) {
        Q_EMIT overlayVisibleChanged();
    }
    if (hadPending) {
        Q_EMIT pendingTextChanged();
    }
    if (hadTrigger) {
        Q_EMIT activeTriggerIdChanged();
    }
}

#include "moc_overlaycontroller.cpp"
