/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QString>
#include <qqmlintegration.h>

class InputEngine;

class EmojiController : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(bool searchActive READ searchActive NOTIFY searchActiveChanged)
    Q_PROPERTY(QString query READ query NOTIFY queryChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)

public:
    explicit EmojiController(InputEngine *inputEngine, QObject *parent = nullptr);
    ~EmojiController() override;

    bool active() const;
    bool searchActive() const;
    QString query() const;
    bool available() const;

    Q_INVOKABLE void open();
    Q_INVOKABLE void close();
    Q_INVOKABLE void startSearch();
    Q_INVOKABLE void stopSearch();
    Q_INVOKABLE void clearQuery();
    Q_INVOKABLE void commitEmoji(const QString &emoji);

    void appendQueryText(const QString &text);
    void removeLastQueryCharacter();

Q_SIGNALS:
    void activeChanged();
    void searchActiveChanged();
    void queryChanged();
    void availableChanged();

private:
    void setQuery(const QString &query);
    void replaceQueryText(const QString &text, int replaceFrom, int replaceLength);

    InputEngine *m_inputEngine = nullptr;
    QString m_query;
    bool m_active = false;
    bool m_searchActive = false;
};
