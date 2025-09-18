/*
    SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <QObject>
#include <QString>
#include <QtGui/private/qxkbcommon_p.h>
#include <QtWaylandClient/QWaylandClientExtensionTemplate>

#include <memory>
#include <qwayland-input-method-unstable-v1.h>
#include <qwayland-wayland.h>

#include "inputplugin.h"

class InputMethodContext;
class Keyboard;

// private implementations, InputPlugin acts as the public facade / multiplexer

class InputMethod : public QWaylandClientExtensionTemplate<InputMethod>, public QtWayland::zwp_input_method_v1
{
    Q_OBJECT
public:
    explicit InputMethod();
    ~InputMethod() override;

    std::shared_ptr<InputMethodContext> current() const
    {
        return m_current;
    }

    bool isActive() const
    {
        return bool(m_current);
    }

Q_SIGNALS:
    void activate();
    void deactivate();
    void activityChanged(bool active);

private:
    void zwp_input_method_v1_activate(struct ::zwp_input_method_context_v1 *id) override;
    void zwp_input_method_v1_deactivate(struct ::zwp_input_method_context_v1 *context) override;

    void setCurrentContext(InputMethodContext *context);
    std::shared_ptr<InputMethodContext> m_current;
};

class InputMethodContext : public QObject, public QtWayland::zwp_input_method_context_v1
{
    Q_OBJECT
public:
    explicit InputMethodContext(struct ::zwp_input_method_context_v1 *id);
    ~InputMethodContext() override;

    std::shared_ptr<Keyboard> keyboard();
    QString m_text;
    uint32_t m_cursor = 0;
    uint32_t m_anchor = 0;
    uint32_t m_latestSerial = 0;
    InputPlugin::ContentHint m_contentHint = InputPlugin::content_hint_none;
    InputPlugin::ContentPurpose m_contentPurpose = InputPlugin::content_purpose_normal;

Q_SIGNALS:
    void reset();
    void contentTypeChanged();
    void preferredLanguageChanged(const QString &language);
    void surroundingTextChanged(const QString &surroundingText, uint32_t cursor, uint32_t anchor);
    void receivedCommit();

private:
    void zwp_input_method_context_v1_surrounding_text(const QString &text, uint32_t cursor, uint32_t anchor) override;
    void zwp_input_method_context_v1_reset() override;
    void zwp_input_method_context_v1_content_type(uint32_t hint, uint32_t purpose) override;
    void zwp_input_method_context_v1_invoke_action(uint32_t button, uint32_t index) override;
    void zwp_input_method_context_v1_commit_state(uint32_t serial) override;
    void zwp_input_method_context_v1_preferred_language(const QString &language) override;

    std::weak_ptr<Keyboard> m_keyboard;
};

class Keyboard : public QObject, public QtWayland::wl_keyboard
{
    Q_OBJECT
public:
    Keyboard(::wl_keyboard *keyboard, InputMethodContext *parent);
    ~Keyboard();

Q_SIGNALS:
    void keyPressed(QKeyEvent *keyEvent);
    void keyReleased(QKeyEvent *keyEvent);

protected:
    void keyboard_keymap(uint32_t format, int32_t fd, uint32_t size) override;
    void keyboard_key(uint32_t serial, uint32_t time, uint32_t key, uint32_t state) override;
    void keyboard_modifiers(uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) override;

private:
    InputMethodContext *m_parent;
    uint32_t mKeymapFormat = WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1;
    QXkbCommon::ScopedXKBContext mXkbContext;
    QXkbCommon::ScopedXKBKeymap mXkbKeymap;
    QXkbCommon::ScopedXKBState mXkbState;
};
