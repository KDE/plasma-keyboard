/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "inputbackend.h"

/**
 * An InputBackend to capture input in-memory like a virtual text field.
 *
 * The InputEngine can be pointed at this backend to have text composers type
 * into a local buffer instead of the client's text field. This can be used to implement
 * input capture for features like emoji search.
 */
class InMemoryInputBackend : public InputBackend
{
    Q_OBJECT

public:
    explicit InMemoryInputBackend(QObject *parent = nullptr);

    /** Committed text followed by the current preedit. */
    QString displayText() const;
    void clear();

    bool isActive() const override;
    Qt::InputMethodHints inputMethodHints() const override;
    QString surroundingText() const override;
    uint32_t cursorPositionUtf8() const override;
    uint32_t anchorPositionUtf8() const override;

    void setPreeditText(const QString &text) override;
    void commitText(const QString &text) override;
    void deleteSurroundingText(int index, int length) override;
    bool sendKeyClick(int key) override;
    bool sendKeyPressed(int key, bool pressed) override;

Q_SIGNALS:
    void displayTextChanged();

private:
    void setText(const QString &text);
    void removePreviousGrapheme();

    QString m_text;
    QString m_preeditText;
};
