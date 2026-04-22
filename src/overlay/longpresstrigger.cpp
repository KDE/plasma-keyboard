/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "longpresstrigger.h"

#include "diacriticsdataloader.h"
#include "inputengine.h"
#include "logging.h"
#include "plasmakeyboardsettings.h"

#include <KLocalizedString>

using namespace Qt::StringLiterals;

LongPressTrigger::LongPressTrigger(InputEngine *inputEngine, QObject *parent)
    : OverlayTrigger(parent)
    , m_inputEngine(inputEngine)
{
    m_diacriticsMap = DiacriticsDataLoader::loadMap(currentLocales());

    if (m_inputEngine) {
        connect(m_inputEngine, &InputEngine::localeChanged, this, &LongPressTrigger::reloadMap);
    }

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
    m_diacriticsMap = DiacriticsDataLoader::loadMap(currentLocales());
    qCDebug(PlasmaKeyboard) << "LongPressTrigger: Diacritics map reloaded for locales" << currentLocales();
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

        // Request timer start for long-press detection.
        // Consume the raw key event to prevent it from being forwarded via
        // wl_keyboard, which would trigger client-side auto-repeat. Instead,
        // the controller commits the base character immediately via
        // commit_string (no repeat) and retracts it if the overlay opens.
        m_timerStarted = true;
        result.action = OverlayAction::StartTimer;
        result.consumeEvent = true;
        result.pendingText = keyEvent->text();
        result.pendingNativeScanCode = keyEvent->nativeScanCode();
        result.timerDurationMs = m_holdThresholdMs;

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

    if (event->isAutoRepeat()) {
        return false;
    }

    // Only handle simple textual keys without control/meta modifiers
    const Qt::KeyboardModifiers mods = event->modifiers();
    const bool modifierAllowed = mods == Qt::NoModifier || mods == Qt::ShiftModifier;
    if (!modifierAllowed) {
        return false;
    }

    // Check if we have diacritics for this character
    const QChar baseChar = event->text().at(0).toLower();
    return m_diacriticsMap.contains(baseChar);
}

QStringList LongPressTrigger::currentLocales() const
{
    if (!m_inputEngine || m_inputEngine->locale().isEmpty()) {
        return {};
    }

    return {m_inputEngine->locale()};
}

#include "moc_longpresstrigger.cpp"
