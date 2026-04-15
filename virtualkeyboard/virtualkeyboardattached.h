/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <qqmlintegration.h>

class InputEngine;
class KeyboardController;
class KeyboardPackageResolver;
class VirtualKeyboardContext;

class VirtualKeyboardAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(InputEngine *inputEngine READ inputEngine NOTIFY contextChanged)
    Q_PROPERTY(KeyboardController *keyboardController READ keyboardController NOTIFY contextChanged)
    Q_PROPERTY(KeyboardPackageResolver *keyboardPackageResolver READ keyboardPackageResolver NOTIFY contextChanged)
    Q_PROPERTY(QObject *alternativeKeysPopup READ alternativeKeysPopup NOTIFY contextChanged)
    Q_PROPERTY(QObject *flickPreviewPopup READ flickPreviewPopup NOTIFY contextChanged)
    Q_PROPERTY(QObject *languagePopup READ languagePopup NOTIFY contextChanged)
    Q_PROPERTY(QObject *inputMethodConnection READ inputMethodConnection NOTIFY contextChanged)

public:
    explicit VirtualKeyboardAttached(QObject *attachedObject);

    InputEngine *inputEngine() const;
    KeyboardController *keyboardController() const;
    KeyboardPackageResolver *keyboardPackageResolver() const;
    QObject *alternativeKeysPopup() const;
    QObject *flickPreviewPopup() const;
    QObject *languagePopup() const;
    QObject *inputMethodConnection() const;

Q_SIGNALS:
    void contextChanged();

private:
    VirtualKeyboardContext *context() const;
    QObject *attachedObjectProperty(const char *name) const;

    QObject *m_attachedObject = nullptr;
};

class VirtualKeyboard : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(VirtualKeyboard)
    QML_UNCREATABLE("VirtualKeyboard is an attached property provider")
    QML_ATTACHED(VirtualKeyboardAttached)

public:
    using QObject::QObject;

    static VirtualKeyboardAttached *qmlAttachedProperties(QObject *object);
};
