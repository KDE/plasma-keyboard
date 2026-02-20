/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "longpresstrigger.h"

#include "logging.h"
#include "plasmakeyboardsettings.h"

#include <KLocalizedString>

using namespace Qt::StringLiterals;

LongPressTrigger::LongPressTrigger(QObject *parent)
    : OverlayTrigger(parent)
{
    // Initialize diacritics map
    // TODO: Load from configuration or locale-aware source?
    m_diacriticsMap.insert(u'a', {u"á"_s, u"à"_s, u"â"_s, u"ä"_s, u"ã"_s, u"å"_s, u"ā"_s, u"ă"_s, u"ą"_s});
    m_diacriticsMap.insert(u'c', {u"ç"_s, u"ć"_s, u"č"_s});
    m_diacriticsMap.insert(u'e', {u"é"_s, u"è"_s, u"ê"_s, u"ë"_s, u"ē"_s, u"ė"_s, u"ę"_s, u"€"_s});
    m_diacriticsMap.insert(u'i', {u"í"_s, u"ì"_s, u"î"_s, u"ï"_s, u"ī"_s, u"į"_s});
    m_diacriticsMap.insert(u'n', {u"ñ"_s, u"ń"_s});
    m_diacriticsMap.insert(u'o', {u"ó"_s, u"ò"_s, u"ô"_s, u"ö"_s, u"õ"_s, u"ō"_s, u"ø"_s});
    m_diacriticsMap.insert(u's', {u"ś"_s, u"š"_s, u"ß"_s});
    m_diacriticsMap.insert(u'u', {u"ú"_s, u"ù"_s, u"û"_s, u"ü"_s, u"ū"_s, u"ů"_s});
    m_diacriticsMap.insert(u'y', {u"ý"_s, u"ÿ"_s});
    m_diacriticsMap.insert(u'z', {u"ź"_s, u"ż"_s, u"ž"_s});
    m_diacriticsMap.insert(u'l', {u"ł"_s});
    m_diacriticsMap.insert(u'g', {u"ğ"_s});
    m_diacriticsMap.insert(u'r', {u"ř"_s});
    m_diacriticsMap.insert(u't', {u"ť"_s});
    m_diacriticsMap.insert(u'd', {u"ď"_s});
    m_diacriticsMap.insert(u'h', {u"ħ"_s});

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
        result.pendingKey = keyEvent->key();
        result.timerDurationMs = m_holdThresholdMs;

        qCDebug(PlasmaKeyboard) << "LongPressTrigger: Requesting timer for" << text << "duration" << m_holdThresholdMs << "ms";
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
            qCDebug(PlasmaKeyboard) << "LongPressTrigger: Timer expired, opening overlay for" << text;
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

    // Only process alphabetic keys with non-empty text
    if (event->text().isEmpty() || !event->text().at(0).isLetter()) {
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

#include "moc_longpresstrigger.cpp"
