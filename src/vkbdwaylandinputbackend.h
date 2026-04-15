/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "inputbackend.h"

class VkbdWaylandInputBackend : public InputBackend
{
    Q_OBJECT

public:
    explicit VkbdWaylandInputBackend(QObject *parent = nullptr);

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

    void setActive(bool active);
    void setInputMethodHints(Qt::InputMethodHints hints);
    void setSurroundingState(const QString &text, uint32_t cursorPositionUtf8, uint32_t anchorPositionUtf8);
Q_SIGNALS:
    void preeditTextRequested(const QString &text);
    void commitTextRequested(const QString &text);
    void deleteSurroundingTextRequested(int index, int length);
    void keyClickRequested(int key, bool *handled);
    void keyPressedRequested(int key, bool pressed, bool *handled);

private:
    bool m_active = false;
    Qt::InputMethodHints m_inputMethodHints = Qt::ImhNone;
    QString m_surroundingText;
    uint32_t m_cursorPositionUtf8 = 0;
    uint32_t m_anchorPositionUtf8 = 0;
};
