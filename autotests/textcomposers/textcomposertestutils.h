/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "inputbackend.h"
#include "inputengine.h"

#include <QLoggingCategory>
#include <QStringList>
#include <QStringView>

Q_LOGGING_CATEGORY(PlasmaKeyboard, "org.kde.plasma.keyboard")

class TextComposerTestBackend : public InputBackend
{
public:
    bool isActive() const override
    {
        return true;
    }

    Qt::InputMethodHints inputMethodHints() const override
    {
        return Qt::ImhNoAutoUppercase;
    }

    QString surroundingText() const override
    {
        return {};
    }

    uint32_t cursorPositionUtf8() const override
    {
        return 0;
    }

    uint32_t anchorPositionUtf8() const override
    {
        return 0;
    }

    void setPreeditText(const QString &text) override
    {
        preeditRequests.append(text);
    }

    void commitText(const QString &text) override
    {
        commitRequests.append(text);
    }

    void deleteSurroundingText(int index, int length) override
    {
        deleteRequests.append({index, length});
    }

    bool sendKeyClick(int key) override
    {
        keyClickRequests.append(key);
        return true;
    }

    bool sendKeyPressed(int key, bool pressed) override
    {
        keyPressedRequests.append({key, pressed});
        setKeyPressed(key, pressed);
        return true;
    }

    QStringList preeditRequests;
    QStringList commitRequests;
    QList<QPair<int, int>> deleteRequests;
    QList<int> keyClickRequests;
    QList<QPair<int, bool>> keyPressedRequests;
};

inline bool sendText(InputEngine &engine, QStringView text)
{
    for (QChar character : text) {
        if (!engine.sendTextComposerKey(Qt::Key_unknown, QString(character))) {
            return false;
        }
    }
    return true;
}
