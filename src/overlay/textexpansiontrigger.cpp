/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "textexpansiontrigger.h"

#include "logging.h"
#include "overlaycontroller.h"
#include "plasmakeyboardsettings.h"

#include <KLocalizedString>

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

TextExpansionTrigger::TextExpansionTrigger(QObject *parent)
    : OverlayTrigger(parent)
{
    // Load the text expansion rules from the settings at initialization.
    loadExpansionRules();
    // Connect to settings changes to reload rules dynamically if needed.
    connect(PlasmaKeyboardSettings::self(), &PlasmaKeyboardSettings::textExpansionRulesChanged, this, &TextExpansionTrigger::loadExpansionRules);
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
    Q_UNUSED(keyEvent)
    Q_UNUSED(controller)

    OverlayTriggerResult result;

    switch (eventType) {
    case OverlayInputEvent::TextBeforeCursorChanged: {
        // `text` is the text before the cursor.
        //
        // Check if the text has an exact match for any known abbreviation.
        const QString abbrev = findMatchingAbbreviation(text);
        if (!abbrev.isEmpty()) {
            result.action = OverlayAction::ReplaceText;
            result.deleteBeforeCursor = abbrev.length();
            result.commitText = expandVariables(m_expansions.value(abbrev));
            break;
        }

        // Check for partial matches and provide candidate expansions.
        const QStringList partialMatches = candidates(text);
        if (!partialMatches.isEmpty()) {
            result.action = OverlayAction::OpenOverlay;
        }
        break;
    }

    case OverlayInputEvent::KeyPress:
    case OverlayInputEvent::KeyRelease:
    case OverlayInputEvent::PreeditChanged:
    case OverlayInputEvent::TimerExpired:
        // Not used
        break;
    }

    return result;
}

QString TextExpansionTrigger::expandVariables(const QString &expansion)
{
    QString result = expansion;
    if (!result.contains(QLatin1Char('{'))) {
        return result;
    }

    const QDateTime now = QDateTime::currentDateTime();
    const QLocale locale;
    result.replace(QStringLiteral("{date}"), locale.toString(now.date(), QLocale::ShortFormat));
    result.replace(QStringLiteral("{time}"), locale.toString(now.time(), QLocale::ShortFormat));
    result.replace(QStringLiteral("{datetime}"), locale.toString(now, QLocale::ShortFormat));
    return result;
}

void TextExpansionTrigger::reset()
{
}

bool TextExpansionTrigger::isEnabled() const
{
    // TODO: Check if plasma-keyboard is enabled instead of hardcoding true?
    return true;
}

QStringList TextExpansionTrigger::candidates(const QString &baseText) const
{
    qCDebug(PlasmaKeyboard) << "TextExpansionTrigger: candidates for base text:" << baseText;

    if (baseText.isEmpty()) {
        return {};
    }

    // Return a list of all partial matches for the base text.
    QStringList matches;
    for (const QString &abbrev : m_expansions.keys()) {
        if (abbrev.startsWith(baseText)) {
            matches.append(m_expansions.value(abbrev));
        }
    }

    qCDebug(PlasmaKeyboard) << "TextExpansionTrigger: partial matches:" << matches;
    return matches;
}

void TextExpansionTrigger::loadExpansionRules()
{
    m_expansions.clear();
    const QJsonDocument rules = QJsonDocument::fromJson(PlasmaKeyboardSettings::self()->textExpansionRules().toUtf8());
    for (const QJsonValue &value : rules.array()) {
        const QJsonObject rule = value.toObject();
        const QString abbreviation = rule.value(QStringLiteral("abbreviation")).toString();
        const QString expansion = rule.value(QStringLiteral("expansion")).toString();
        if (!abbreviation.isEmpty() && !expansion.isEmpty()) {
            m_expansions.insert(abbreviation, expansion);
        }
    }
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
