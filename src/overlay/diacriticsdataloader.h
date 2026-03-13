/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QHash>
#include <QList>
#include <QStringList>

class QChar;

/**
 * @file
 *
 * Static utility class for loading and merging locale-aware diacritics maps
 * from JSON data files.
 *
 * Discovers data files via QStandardPaths using the path
 * "plasma/keyboard/diacritics/". Falls back to compiled-in Qt resource files
 * when filesystem files are not found (requires diacritics.qrc to be present).
 *
 * Two levels of data files are supported:
 * - A base file ("base.json") with a pan-European Latin-script superset plus
 *   symbol alternates (currency, punctuation, arrows, typographic quotes).
 * - Per-locale files (e.g., "de_DE.json") that reorder or extend the base map
 *   for a specific locale. Keys absent from a locale file fall through to the
 *   base map's candidates.
 */

/**
 * Load and merge locale-aware diacritics maps from JSON data files.
 *
 * All methods are static; this class is not intended to be instantiated.
 * The primary entry point is loadMap(), which handles discovery, parsing,
 * and merging in a single call.
 */
class DiacriticsDataLoader
{
public:
    /**
     * Load and merge the diacritics map for the given enabled locales.
     *
     * Returns a map from base character to an ordered list of diacritic
     * candidates. When @p enabledLocales is empty, returns the base map
     * unchanged.
     *
     * @param enabledLocales Ordered list of enabled locale codes (e.g.,
     *        {"de_DE", "fr_FR"}). The first entry is the primary locale whose
     *        ordering takes precedence during merging.
     * @return Merged QHash mapping QChar keys to ordered candidate QStringList
     *         values. Returns an empty hash if neither the base file on disk
     *         nor the Qt resource fallback could be loaded.
     */
    static QHash<QChar, QStringList> loadMap(const QStringList &enabledLocales);

    /**
     * Load the base diacritics map from the "base.json" data file.
     *
     * Looks up the file via QStandardPaths (GenericDataLocation,
     * "plasma/keyboard/diacritics/base.json") first. Falls back to the
     * compiled-in Qt resource copy at ":/diacritics/base.json" if the
     * filesystem file is not found.
     *
     * Logs a warning if neither the filesystem nor the resource file is
     * available.
     *
     * @return QHash mapping QChar keys to candidate QStringList values, or
     *         an empty hash if neither source is available.
     */
    static QHash<QChar, QStringList> loadBaseMap();

    /**
     * Load the locale-specific diacritics map for the given locale.
     *
     * The locale file is expected at
     * "plasma/keyboard/diacritics/<locale>.json" under GenericDataLocation.
     * Only keys whose ordering or candidates differ from the base need to be
     * specified; absent keys fall through to the base map during merging.
     *
     * Logs a warning if the locale file is not found for an enabled locale
     * (not an error; gracefully falls back to the base map for that locale).
     *
     * @param locale BCP 47-style locale code (e.g., "de_DE").
     * @return QHash mapping QChar keys to candidate QStringList values, or
     *         an empty hash if the locale file is not found or cannot be
     *         parsed.
     */
    static QHash<QChar, QStringList> loadLocaleMap(const QString &locale);

    /**
     * Merge a base map with zero or more locale-specific maps.
     *
     * For each character key, candidates are ordered as follows:
     * 1. Candidates from the primary (first) locale map, in its order.
     * 2. New candidates from each subsequent locale map, in list order,
     *    excluding duplicates already seen.
     * 3. Remaining candidates from the base map, excluding duplicates.
     *
     * The union of all candidates across all maps is preserved; no candidate
     * is silently dropped.
     *
     * @param base       The base diacritics map (pan-European superset).
     * @param localeMaps Locale maps in priority order (primary locale first).
     * @return Merged QHash with the union of all candidates, ordered by
     *         primary-locale priority.
     */
    static QHash<QChar, QStringList> mergeMaps(const QHash<QChar, QStringList> &base, const QList<QHash<QChar, QStringList>> &localeMaps);

private:
    /**
     * Parse a diacritics JSON file at the given path.
     *
     * @param filePath Absolute filesystem path to the JSON file.
     * @return Parsed QHash, or an empty hash if the file cannot be opened or
     *         parsed.
     */
    static QHash<QChar, QStringList> parseJsonFile(const QString &filePath);

    /**
     * Parse raw JSON bytes into a diacritics QHash.
     *
     * Validates the "version" field (must equal 1) and iterates the "map"
     * object. Each key must be a single character; multi-character keys, empty
     * candidate strings, and candidates containing non-printable characters
     * are logged as warnings and skipped.
     *
     * @param content    Raw JSON bytes.
     * @param sourceName Descriptive label used in log messages (e.g. file
     *                   path or resource path).
     * @return Parsed QHash, or an empty hash on version mismatch or JSON
     *         parse error.
     */
    static QHash<QChar, QStringList> parseJsonContent(const QByteArray &content, const QString &sourceName);
};
