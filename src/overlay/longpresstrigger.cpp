/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "longpresstrigger.h"

#include "diacriticsdataloader.h"
#include "logging.h"
#include "plasmakeyboardsettings.h"

#include <KLocalizedString>

using namespace Qt::StringLiterals;

LongPressTrigger::LongPressTrigger(QObject *parent)
    : OverlayTrigger(parent)
{
    m_diacriticsMap = DiacriticsDataLoader::loadMap(PlasmaKeyboardSettings::self()->enabledLocales());

    // Reload the diacritics map whenever the user changes the enabled locales
    // in the KCM, so the keyboard reflects the new locale ordering without
    // requiring a restart.
    connect(PlasmaKeyboardSettings::self(), &PlasmaKeyboardSettings::enabledLocalesChanged, this, &LongPressTrigger::reloadMap);

    m_holdThresholdMs = PlasmaKeyboardSettings::self()->diacriticsHoldThresholdMs();
}

QString LongPressTrigger::triggerId() const
{
    return QStringLiteral("diacritics");
}

QString LongPressTrigger::displayName() const
{
    return i18nc("@label Name of the diacritics overlay trigger", "Diacritics (Long Press)");
}

void LongPressTrigger::reloadMap()
{
    m_diacriticsMap = DiacriticsDataLoader::loadMap(PlasmaKeyboardSettings::self()->enabledLocales());
    qCDebug(PlasmaKeyboard) << "LongPressTrigger: Diacritics map reloaded for locales" << PlasmaKeyboardSettings::self()->enabledLocales();
}

// clang-format off
OverlayTriggerResult LongPressTrigger::processEvent(OverlayInputEvent eventType,
                                                          const QKeyEvent *keyEvent,
                                                          const QString &text,
                                                          OverlayController *controller)
// clang-format on
{
    Q_UNUSED(controller)

    OverlayTriggerResult result;

    switch (eventType) {
    case OverlayInputEvent::KeyPress: {
        if (!keyEvent || !shouldHandleKey(keyEvent)) {
            return result;
        }

        // Characters with diacritic candidates enter the long-press hold timer
        // (consumed, blocking native auto-repeat). Printable characters without
        // diacritics are consumed and committed via commit_string too, but skip
        // the pending state so the controller's repeat-suppression check does
        // not catch them.
        const QChar baseChar = keyEvent->text().at(0).toLower();
        if (m_diacriticsMap.contains(baseChar)) {
            if (keyEvent->isAutoRepeat()) {
                // Diacritic auto-repeat: let the controller's pending-key check
                // suppress it; don't consume here, so the event falls through.
                return result;
            }
            m_timerStarted = true;
            result.action = OverlayAction::StartTimer;
            result.consumeEvent = true;
            result.pendingText = keyEvent->text();
            result.pendingNativeScanCode = keyEvent->nativeScanCode();
            result.timerDurationMs = m_holdThresholdMs;
        } else {
            // Printable character without diacritics: consume and commit via
            // commit_string for ordering consistency. Since we are accepting the key
            // event, ask the controller to synthesise repeats with its own internal
            // timer to retain native key repeat behavior.
            result.action = OverlayAction::ConsumeEvent;
            result.consumeEvent = true;
            result.commitText = keyEvent->text();
            result.enableRepeat = true;
            result.repeatScanCode = keyEvent->nativeScanCode();
            result.pendingNativeScanCode = keyEvent->nativeScanCode();
        }

        // qCDebug(PlasmaKeyboard) << "LongPressTrigger: Requesting timer for" << text << "duration" << m_holdThresholdMs << "ms";
        break;
    }

    case OverlayInputEvent::TimerExpired: {
        if (!m_timerStarted) {
            return result;
        }

        // Timer expired, request overlay
        const auto candidateList = this->candidates(text);
        if (!candidateList.isEmpty()) {
            result.action = OverlayAction::OpenOverlay;
            // qCDebug(PlasmaKeyboard) << "LongPressTrigger: Timer expired, opening overlay for" << text;
        }
        m_timerStarted = false;
        break;
    }

    case OverlayInputEvent::KeyRelease:
    case OverlayInputEvent::PreeditChanged:
    case OverlayInputEvent::TextCommitted:
        // These are handled by the controller
        break;
    }

    return result;
}

void LongPressTrigger::reset()
{
    m_timerStarted = false;
}

bool LongPressTrigger::isEnabled() const
{
    return PlasmaKeyboardSettings::self()->diacriticsPopupEnabled();
}

QStringList LongPressTrigger::candidates(const QString &baseText) const
{
    if (baseText.isEmpty()) {
        return {};
    }

    const QChar baseChar = baseText.at(0).toLower();
    const auto it = m_diacriticsMap.find(baseChar);
    if (it == m_diacriticsMap.end()) {
        return {};
    }

    QStringList result = it.value();

    // Preserve case
    if (baseText.at(0).isUpper()) {
        for (auto &s : result) {
            s = s.toUpper();
        }
    }

    return result;
}

void LongPressTrigger::setHoldThreshold(int ms)
{
    m_holdThresholdMs = ms;
}

int LongPressTrigger::holdThreshold() const
{
    return m_holdThresholdMs;
}

bool LongPressTrigger::shouldHandleKey(const QKeyEvent *event) const
{
    if (!event) {
        return false;
    }

    // Never treat backspace/delete as diacritics candidates
    if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        return false;
    }

    if (event->text().isEmpty()) {
        return false;
    }

    // Only handle simple textual keys without control/meta modifiers
    const Qt::KeyboardModifiers mods = event->modifiers();
    const bool modifierAllowed = mods == Qt::NoModifier || mods == Qt::ShiftModifier;
    if (!modifierAllowed) {
        return false;
    }

    // Intercept all printable characters so they are consistently committed via
    // commit_string, avoiding ordering issues between commit and key event paths
    // (e.g. when a password manager injects key events, if they arrive via multiple
    // pathways the ordering could be inconsistent). See:
    // https://invent.kde.org/plasma/plasma-keyboard/-/merge_requests/131
    const QChar baseChar = event->text().at(0).toLower();
    return baseChar.isPrint();
}

#include "moc_longpresstrigger.cpp"
