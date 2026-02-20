/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "textexpansiontrigger.h"

#include "logging.h"
#include "overlaycontroller.h"

#include <KLocalizedString>

TextExpansionTrigger::TextExpansionTrigger(QObject *parent)
    : OverlayTrigger(parent)
{
    // TODO: Load expansions from settings
}

QString TextExpansionTrigger::triggerId() const
{
    return QStringLiteral("textexpansion");
}

QString TextExpansionTrigger::displayName() const
{
    return i18nc("@label Name of the text expansion overlay trigger", "Text Expansion");
}

// clang-format off
OverlayTriggerResult TextExpansionTrigger::processEvent(OverlayInputEvent eventType,
                                                              const QKeyEvent *keyEvent,
                                                              const QString &text,
                                                              OverlayController *controller)
// clang-format on
{
    Q_UNUSED(controller)

    OverlayTriggerResult result;

    switch (eventType) {
    case OverlayInputEvent::TextCommitted: {
        // Check if committed text ends with an abbreviation
        const QString abbrev = findMatchingAbbreviation(text);
        if (!abbrev.isEmpty()) {
            if (m_requiresTriggerKey) {
                // Store pending abbreviation, wait for trigger key
                m_pendingAbbreviation = abbrev;
                qCDebug(PlasmaKeyboard) << "TextExpansionTrigger: Detected abbreviation, waiting for trigger key:" << abbrev;
            } else {
                // Expand immediately
                result.action = OverlayAction::ReplaceText;
                result.deleteBeforeCursor = abbrev.length();
                result.commitText = m_expansions.value(abbrev);
                qCDebug(PlasmaKeyboard) << "TextExpansionTrigger: Expanding" << abbrev << "to" << result.commitText;
            }
        }
        break;
    }

    case OverlayInputEvent::KeyPress: {
        if (!m_pendingAbbreviation.isEmpty() && keyEvent && keyEvent->key() == m_triggerKey) {
            // Trigger key pressed, expand
            result.action = OverlayAction::ReplaceText;
            result.deleteBeforeCursor = m_pendingAbbreviation.length();
            result.commitText = m_expansions.value(m_pendingAbbreviation);
            result.consumeEvent = true;
            qCDebug(PlasmaKeyboard) << "TextExpansionTrigger: Trigger key pressed, expanding" << m_pendingAbbreviation;
            m_pendingAbbreviation.clear();
        } else if (!m_pendingAbbreviation.isEmpty()) {
            // Different key pressed, cancel pending expansion
            m_pendingAbbreviation.clear();
        }
        break;
    }

    case OverlayInputEvent::KeyRelease:
    case OverlayInputEvent::PreeditChanged:
    case OverlayInputEvent::TimerExpired:
        // Not used
        break;
    }

    return result;
}

void TextExpansionTrigger::reset()
{
    m_pendingAbbreviation.clear();
}

bool TextExpansionTrigger::isEnabled() const
{
    // TODO: Add setting for text expansion
    // return PlasmaKeyboardSettings::self()->textExpansionEnabled();
    return false; // Disabled until expansion data is implemented
}

QStringList TextExpansionTrigger::candidates(const QString &baseText) const
{
    // Text expansion doesn't show candidates in a popup (it replaces directly)
    // But we could show matching expansions if we wanted preview
    Q_UNUSED(baseText)
    return {};
}

void TextExpansionTrigger::addExpansion(const QString &abbreviation, const QString &expansion)
{
    m_expansions.insert(abbreviation, expansion);
}

void TextExpansionTrigger::removeExpansion(const QString &abbreviation)
{
    m_expansions.remove(abbreviation);
}

void TextExpansionTrigger::setRequiresTriggerKey(bool value)
{
    m_requiresTriggerKey = value;
}

bool TextExpansionTrigger::requiresTriggerKey() const
{
    return m_requiresTriggerKey;
}

void TextExpansionTrigger::setTriggerKey(Qt::Key key)
{
    m_triggerKey = key;
}

Qt::Key TextExpansionTrigger::triggerKey() const
{
    return m_triggerKey;
}

QString TextExpansionTrigger::findMatchingAbbreviation(const QString &text) const
{
    // Check if text ends with any known abbreviation
    // We check longest matches first for proper precedence
    QList<QString> abbrevs = m_expansions.keys();
    std::sort(abbrevs.begin(), abbrevs.end(), [](const QString &a, const QString &b) {
        return a.length() > b.length();
    });

    for (const auto &abbrev : abbrevs) {
        if (text.endsWith(abbrev)) {
            // Ensure it's a word boundary (start of string or preceded by space)
            const int pos = text.length() - abbrev.length();
            if (pos == 0 || text.at(pos - 1).isSpace()) {
                return abbrev;
            }
        }
    }

    return {};
}

#include "moc_textexpansiontrigger.cpp"
