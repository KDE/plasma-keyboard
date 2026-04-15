/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "virtualkeyboardattached.h"

#include "inputengine.h"
#include "keyboardcontroller.h"
#include "keyboardpackageresolver.h"
#include "virtualkeyboardcontext.h"

#include <QQuickItem>
#include <QQuickWindow>
#include <QTimer>
#include <QVariant>

static VirtualKeyboardContext *contextFromObject(QObject *object)
{
    if (!object) {
        return nullptr;
    }

    const QVariant contextProperty = object->property("virtualKeyboardContext");
    if (auto *keyboardContext = qvariant_cast<VirtualKeyboardContext *>(contextProperty)) {
        return keyboardContext;
    }
    return qobject_cast<VirtualKeyboardContext *>(qvariant_cast<QObject *>(contextProperty));
}

static QObject *objectProperty(QObject *object, const char *name)
{
    if (!object) {
        return nullptr;
    }

    const QVariant property = object->property(name);
    if (auto *propertyObject = qvariant_cast<QObject *>(property)) {
        return propertyObject;
    }
    return nullptr;
}

VirtualKeyboardAttached::VirtualKeyboardAttached(QObject *attachedObject)
    : QObject(attachedObject)
    , m_attachedObject(attachedObject)
{
    if (auto *item = qobject_cast<QQuickItem *>(attachedObject)) {
        connect(item, &QQuickItem::parentChanged, this, &VirtualKeyboardAttached::contextChanged);
        connect(item, &QQuickItem::windowChanged, this, &VirtualKeyboardAttached::contextChanged);
    }

    QTimer::singleShot(0, this, &VirtualKeyboardAttached::contextChanged);
}

InputEngine *VirtualKeyboardAttached::inputEngine() const
{
    auto *keyboardContext = context();
    return keyboardContext ? keyboardContext->inputEngine() : nullptr;
}

KeyboardController *VirtualKeyboardAttached::keyboardController() const
{
    auto *keyboardContext = context();
    return keyboardContext ? keyboardContext->keyboardController() : nullptr;
}

KeyboardPackageResolver *VirtualKeyboardAttached::keyboardPackageResolver() const
{
    auto *keyboardContext = context();
    return keyboardContext ? keyboardContext->keyboardPackageResolver() : nullptr;
}

QObject *VirtualKeyboardAttached::alternativeKeysPopup() const
{
    return attachedObjectProperty("alternativeKeysPopupItem");
}

QObject *VirtualKeyboardAttached::flickPreviewPopup() const
{
    return attachedObjectProperty("flickPreviewPopupItem");
}

QObject *VirtualKeyboardAttached::languagePopup() const
{
    return attachedObjectProperty("languagePopupItem");
}

QObject *VirtualKeyboardAttached::inputMethodConnection() const
{
    return attachedObjectProperty("inputMethodConnection");
}

VirtualKeyboardContext *VirtualKeyboardAttached::context() const
{
    for (QObject *object = m_attachedObject; object; object = object->parent()) {
        if (auto *keyboardContext = contextFromObject(object)) {
            return keyboardContext;
        }

        auto *item = qobject_cast<QQuickItem *>(object);
        if (!item) {
            continue;
        }

        for (QQuickItem *parentItem = item->parentItem(); parentItem; parentItem = parentItem->parentItem()) {
            if (auto *keyboardContext = contextFromObject(parentItem)) {
                return keyboardContext;
            }
        }

        if (auto *keyboardContext = contextFromObject(item->window())) {
            return keyboardContext;
        }
    }

    return nullptr;
}

QObject *VirtualKeyboardAttached::attachedObjectProperty(const char *name) const
{
    for (QObject *object = m_attachedObject; object; object = object->parent()) {
        if (auto *propertyObject = objectProperty(object, name)) {
            return propertyObject;
        }

        auto *item = qobject_cast<QQuickItem *>(object);
        if (!item) {
            continue;
        }

        for (QQuickItem *parentItem = item->parentItem(); parentItem; parentItem = parentItem->parentItem()) {
            if (auto *propertyObject = objectProperty(parentItem, name)) {
                return propertyObject;
            }
        }

        if (auto *propertyObject = objectProperty(item->window(), name)) {
            return propertyObject;
        }
    }

    return nullptr;
}

VirtualKeyboardAttached *VirtualKeyboard::qmlAttachedProperties(QObject *object)
{
    return new VirtualKeyboardAttached(object);
}
