/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QAbstractListModel>
#include <qqmlintegration.h>

/**
 * Model providing candidate items for overlay popups.
 *
 * This model is used by overlay views (diacritics, emoji, text expansion) to display
 * selectable candidates. It supports filtering via the query property.
 */
class CandidateModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * Filter query for narrowing candidates (e.g., ":whale" for emoji).
     */
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

    /**
     * The trigger type that populated this model.
     */
    Q_PROPERTY(QString triggerId READ triggerId WRITE setTriggerId NOTIFY triggerIdChanged)

public:
    /**
     * Roles for accessing candidate data from QML.
     */
    enum Roles {
        /** Display text shown in the popup (e.g., "á" or "🐋"). */
        DisplayRole = Qt::UserRole + 1,
        /** Text to insert when candidate is selected. */
        InsertTextRole,
        /** Optional description (e.g., "whale" for emoji). */
        DescriptionRole,
        /** Category for grouping (e.g., "Animals" for emoji). */
        CategoryRole,
        /** Search keywords for filtering. */
        KeywordsRole,
    };
    Q_ENUM(Roles)

    explicit CandidateModel(QObject *parent = nullptr);
    ~CandidateModel() override = default;

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString query() const;
    void setQuery(const QString &query);

    QString triggerId() const;
    void setTriggerId(const QString &triggerId);

    /**
     * Set the candidate list directly.
     *
     * @param candidates List of candidate display strings.
     */
    void setCandidates(const QStringList &candidates);

    /**
     * Clear all candidates.
     */
    void clear();

    /**
     * Get the insert text for a candidate at the given index.
     *
     * @param index Row index.
     * @return The text to insert, or empty string if invalid.
     */
    Q_INVOKABLE QString insertTextAt(int index) const;

Q_SIGNALS:
    void queryChanged();
    void triggerIdChanged();

private:
    struct Candidate {
        QString display;
        QString insertText;
        QString description;
        QString category;
        QStringList keywords;
    };

    QList<Candidate> m_candidates;
    QString m_query;
    QString m_triggerId;
};
