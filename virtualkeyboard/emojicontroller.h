/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QPointer>
#include <QString>
#include <qqmlintegration.h>

class InputBackend;
class InputEngine;
class InMemoryInputBackend;

/**
 * Controls the emoji picker and emoji search.
 *
 * When in "emoji search" mode, the input engine's backend is swapped out so
 * we can capture input here (instead of it going to the app).
 */
class EmojiController : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(bool searchActive READ searchActive NOTIFY searchActiveChanged)
    Q_PROPERTY(QString query READ query NOTIFY queryChanged)
    Q_PROPERTY(int queryCursorPosition READ queryCursorPosition NOTIFY queryCursorPositionChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)

public:
    explicit EmojiController(InputEngine *inputEngine, QObject *parent = nullptr);
    ~EmojiController() override;

    bool active() const;
    bool searchActive() const;
    QString query() const;
    int queryCursorPosition() const;
    bool available() const;

    Q_INVOKABLE void open();
    Q_INVOKABLE void close();
    Q_INVOKABLE void startSearch();
    Q_INVOKABLE void stopSearch();
    Q_INVOKABLE void clearQuery();
    Q_INVOKABLE void commitEmoji(const QString &emoji);

Q_SIGNALS:
    void activeChanged();
    void searchActiveChanged();
    void queryChanged();
    void queryCursorPositionChanged();
    void availableChanged();

private:
    QPointer<InputEngine> m_inputEngine;

    // InputEngine's existing InputBackend
    InputBackend *m_clientBackend = nullptr;
    // InputBackend to swap-in to capture input from the engine for emoji search
    InMemoryInputBackend *m_searchBackend = nullptr;

    bool m_active = false;
};
