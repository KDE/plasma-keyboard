/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QList>
#include <QObject>
#include <QString>

class InputBackend : public QObject
{
    Q_OBJECT

public:
    explicit InputBackend(QObject *parent = nullptr);
    virtual ~InputBackend();

    virtual bool isActive() const = 0;
    virtual Qt::InputMethodHints inputMethodHints() const = 0;
    virtual QString surroundingText() const = 0;
    virtual uint32_t cursorPositionUtf8() const = 0;
    virtual uint32_t anchorPositionUtf8() const = 0;
    QList<int> pressedKeys() const;

    virtual void setPreeditText(const QString &text) = 0;
    virtual void commitText(const QString &text) = 0;
    virtual void deleteSurroundingText(int index, int length) = 0;
    virtual bool sendKeyClick(int key) = 0;
    virtual bool sendKeyPressed(int key, bool pressed) = 0;
    void clearPressedKeys();
    void reset();

Q_SIGNALS:
    void activeChanged();
    void surroundingTextChanged();
    void inputMethodHintsChanged();
    void pressedKeysChanged();
    void resetRequested();

protected:
    void setKeyPressed(int key, bool pressed);

private:
    QList<int> m_pressedKeys;
};
