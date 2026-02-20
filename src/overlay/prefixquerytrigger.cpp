/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "prefixquerytrigger.h"

#include "logging.h"

#include <KLocalizedString>

PrefixQueryTrigger::PrefixQueryTrigger(QObject *parent)
    : OverlayTrigger(parent)
{
}

QString PrefixQueryTrigger::triggerId() const
{
    return QStringLiteral("emoji");
}

QString PrefixQueryTrigger::displayName() const
{
    return i18nc("@label Name of the emoji overlay trigger", "Emoji (Prefix Search)");
}

// clang-format off
OverlayTriggerResult PrefixQueryTrigger::processEvent(OverlayInputEvent eventType,
                                                            const QKeyEvent *keyEvent,
                                                            const QString &text,
                                                            OverlayController *controller)
// clang-format on
{
    Q_UNUSED(keyEvent)
    Q_UNUSED(controller)

    OverlayTriggerResult result;

    switch (eventType) {
    case OverlayInputEvent::PreeditChanged:
    case OverlayInputEvent::TextCommitted: {
        // Look for prefix pattern in text
        const QString query = extractQuery(text);
        if (!query.isEmpty() && query.length() >= m_minQueryLength) {
            m_currentQuery = query;
            m_active = true;
            result.action = OverlayAction::OpenOverlay;
            result.query = query;
            qCDebug(PlasmaKeyboard) << "PrefixQueryTrigger: Detected query" << query;
        } else if (m_active && query.isEmpty()) {
            // Prefix was removed, close overlay
            m_active = false;
            m_currentQuery.clear();
            result.action = OverlayAction::CloseOverlay;
        }
        break;
    }

    case OverlayInputEvent::KeyPress:
    case OverlayInputEvent::KeyRelease:
    case OverlayInputEvent::TimerExpired:
        // Not used for prefix query trigger
        break;
    }

    return result;
}

void PrefixQueryTrigger::reset()
{
    m_active = false;
    m_currentQuery.clear();
}

bool PrefixQueryTrigger::isEnabled() const
{
    // TODO: Add setting for emoji popup
    // return PlasmaKeyboardSettings::self()->emojiPopupEnabled();
    return false; // Disabled until emoji data is implemented
}

QStringList PrefixQueryTrigger::candidates(const QString &baseText) const
{
    Q_UNUSED(baseText)
    // TODO: Implement emoji search
    // This will query an emoji database filtered by m_currentQuery
    return {};
}

void PrefixQueryTrigger::setPrefix(QChar prefix)
{
    m_prefix = prefix;
}

QChar PrefixQueryTrigger::prefix() const
{
    return m_prefix;
}

void PrefixQueryTrigger::setMinQueryLength(int length)
{
    m_minQueryLength = length;
}

int PrefixQueryTrigger::minQueryLength() const
{
    return m_minQueryLength;
}

QString PrefixQueryTrigger::extractQuery(const QString &text) const
{
    // Find the last occurrence of the prefix
    const int prefixPos = text.lastIndexOf(m_prefix);
    if (prefixPos < 0) {
        return {};
    }

    // Extract everything after the prefix until space or end
    const int queryStart = prefixPos + 1;
    if (queryStart >= text.length()) {
        return {};
    }

    QString query;
    for (int i = queryStart; i < text.length(); ++i) {
        const QChar c = text.at(i);
        if (c.isSpace()) {
            break;
        }
        query += c;
    }

    return query;
}

#include "moc_prefixquerytrigger.cpp"
