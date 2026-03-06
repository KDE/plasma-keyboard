/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "diacriticsdataloader.h"

#include "logging.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QSet>
#include <QStandardPaths>

using namespace Qt::StringLiterals;

/// The only data-file format version this loader understands.
/// Files declaring a different version will be rejected with a warning.
static constexpr int kExpectedVersion = 1;

/// Relative path beneath GenericDataLocation where all diacritics JSON files are expected
/// (e.g. `~/.local/share/plasma/keyboard/diacritics/` or the installed `/usr/share/` equivalent).
static const QString kDataSubPath = u"plasma/keyboard/diacritics/"_s;

/// File name of the language-neutral base diacritics map, located under kDataSubPath.
/// This file defines the default set of alternates for every supported base character.
static const QString kBaseFileName = u"base.json"_s;

/// Path to the compiled-in Qt resource that serves as a last-resort fallback
/// when base.json cannot be found on the filesystem.
static const QString kResourceBasePath = u":/diacritics/base.json"_s;

// ── Public API ───────────────────────────────────────────────────────────────

QHash<QChar, QStringList> DiacriticsDataLoader::loadMap(const QStringList &enabledLocales)
{
    const QHash<QChar, QStringList> base = loadBaseMap();

    if (enabledLocales.isEmpty()) {
        return base;
    }

    QList<QHash<QChar, QStringList>> localeMaps;
    localeMaps.reserve(enabledLocales.size());
    for (const QString &locale : enabledLocales) {
        localeMaps.append(loadLocaleMap(locale));
    }

    return mergeMaps(base, localeMaps);
}

QHash<QChar, QStringList> DiacriticsDataLoader::loadBaseMap()
{
    // Try GenericDataLocation first (e.g., ~/.local/share/plasma/keyboard/diacritics/base.json or the installed equivalent).
    const QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, kDataSubPath + kBaseFileName);
    if (!path.isEmpty()) {
        const QHash<QChar, QStringList> result = parseJsonFile(path);
        if (!result.isEmpty()) {
            qCDebug(PlasmaKeyboard) << "DiacriticsDataLoader: Loaded base map from" << path;
            return result;
        }
    }

    // Fallback: compiled-in Qt resource (populated by diacritics.qrc).
    QFile resourceFile(kResourceBasePath);
    if (resourceFile.open(QIODevice::ReadOnly)) {
        const QHash<QChar, QStringList> result = parseJsonContent(resourceFile.readAll(), kResourceBasePath);
        if (!result.isEmpty()) {
            qCDebug(PlasmaKeyboard) << "DiacriticsDataLoader: Loaded base map from Qt resource";
            return result;
        }
    }

    qCWarning(PlasmaKeyboard) << "DiacriticsDataLoader: Could not load base.json from filesystem or Qt resource;"
                                 " diacritics will be unavailable until data files are installed.";
    return {};
}

QHash<QChar, QStringList> DiacriticsDataLoader::loadLocaleMap(const QString &locale)
{
    const QString fileName = locale + u".json"_s;
    const QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, kDataSubPath + fileName);

    if (path.isEmpty()) {
        qCInfo(PlasmaKeyboard) << "DiacriticsDataLoader: No diacritics file found for enabled locale" << locale
                               << "(falling back to base map ordering for this locale)";
        return {};
    }

    const QHash<QChar, QStringList> result = parseJsonFile(path);
    if (result.isEmpty()) {
        qCWarning(PlasmaKeyboard) << "DiacriticsDataLoader: Failed to parse locale diacritics file for" << locale;
    } else {
        qCDebug(PlasmaKeyboard) << "DiacriticsDataLoader: Loaded locale map for" << locale << "from" << path;
    }
    return result;
}

QHash<QChar, QStringList> DiacriticsDataLoader::mergeMaps(const QHash<QChar, QStringList> &base, const QList<QHash<QChar, QStringList>> &localeMaps)
{
    // Collect the union of all character keys across base and all locale maps.
    QSet<QChar> allKeys;
    allKeys.reserve(base.size());
    for (auto it = base.cbegin(); it != base.cend(); ++it) {
        allKeys.insert(it.key());
    }
    for (const auto &localeMap : localeMaps) {
        for (auto it = localeMap.cbegin(); it != localeMap.cend(); ++it) {
            allKeys.insert(it.key());
        }
    }

    QHash<QChar, QStringList> merged;
    merged.reserve(allKeys.size());

    for (const QChar key : std::as_const(allKeys)) {
        QStringList result;
        QSet<QString> seen;

        // 1. Walk locale maps in priority order (primary locale first).
        //    Append candidates that haven't been seen yet.
        for (const auto &localeMap : localeMaps) {
            const QStringList localeCandidates = localeMap.value(key);
            for (const QString &candidate : localeCandidates) {
                if (!seen.contains(candidate)) {
                    seen.insert(candidate);
                    result.append(candidate);
                }
            }
        }

        // 2. Append remaining base candidates that haven't been seen yet.
        const QStringList baseCandidates = base.value(key);
        for (const QString &candidate : baseCandidates) {
            if (!seen.contains(candidate)) {
                seen.insert(candidate);
                result.append(candidate);
            }
        }

        if (!result.isEmpty()) {
            merged.insert(key, result);
        }
    }

    return merged;
}

// ── Private helpers ───────────────────────────────────────────────────────────

QHash<QChar, QStringList> DiacriticsDataLoader::parseJsonFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(PlasmaKeyboard) << "DiacriticsDataLoader: Cannot open file:" << filePath;
        return {};
    }
    return parseJsonContent(file.readAll(), filePath);
}

QHash<QChar, QStringList> DiacriticsDataLoader::parseJsonContent(const QByteArray &content, const QString &sourceName)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(content, &parseError);
    if (doc.isNull() || parseError.error != QJsonParseError::NoError) {
        qCWarning(PlasmaKeyboard) << "DiacriticsDataLoader: JSON parse error in" << sourceName << "at offset" << parseError.offset << ":"
                                  << parseError.errorString();
        return {};
    }

    const QJsonObject root = doc.object();

    // Validate the version field.
    const int version = root.value("version"_L1).toInt();
    if (version != kExpectedVersion) {
        qCWarning(PlasmaKeyboard) << "DiacriticsDataLoader: Unsupported version" << version << "in" << sourceName << "(expected" << kExpectedVersion << ")";
        return {};
    }

    const QJsonObject mapObj = root.value("map"_L1).toObject();
    if (mapObj.isEmpty()) {
        qCWarning(PlasmaKeyboard) << "DiacriticsDataLoader: Empty or missing 'map' object in" << sourceName;
        return {};
    }

    QHash<QChar, QStringList> result;
    result.reserve(mapObj.size());

    for (auto it = mapObj.constBegin(); it != mapObj.constEnd(); ++it) {
        const QString keyStr = it.key();
        if (keyStr.size() != 1) {
            qCWarning(PlasmaKeyboard) << "DiacriticsDataLoader: Map key must be a single character, got:" << keyStr << "in" << sourceName << "(skipping)";
            continue;
        }

        const QChar key = keyStr.at(0);
        const QJsonArray candidatesArray = it.value().toArray();

        QStringList candidates;
        candidates.reserve(candidatesArray.size());
        for (const QJsonValue &val : candidatesArray) {
            const QString candidate = val.toString();
            if (candidate.isEmpty()) {
                qCWarning(PlasmaKeyboard) << "DiacriticsDataLoader: Empty candidate string for key" << keyStr << "in" << sourceName << "(skipping)";
                continue;
            }

            // Validate that all characters in the candidate are printable.
            // Non-printable characters (control characters, etc.) cannot be
            // meaningfully displayed as keyboard alternates.
            bool hasNonPrintable = false;
            for (const QChar ch : candidate) {
                if (!ch.isPrint()) {
                    hasNonPrintable = true;
                    break;
                }
            }
            if (hasNonPrintable) {
                qCWarning(PlasmaKeyboard) << "DiacriticsDataLoader: Candidate" << candidate << "for key" << keyStr << "in" << sourceName
                                          << "contains non-printable characters (skipping)";
                continue;
            }

            candidates.append(candidate);
        }

        if (!candidates.isEmpty()) {
            result.insert(key, candidates);
        } else {
            qCWarning(PlasmaKeyboard) << "DiacriticsDataLoader: No valid candidates for key" << keyStr << "in" << sourceName;
        }
    }

    return result;
}
