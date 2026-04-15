/*
    SPDX-FileCopyrightText: 2026 Abenezer Wesenseged <wseged@proton.me>
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "../directtextcomposer.h"

class AmharicTextComposer : public DirectTextComposer
{
    Q_OBJECT
    QML_NAMED_ELEMENT(AmharicTextComposer)

public:
    explicit AmharicTextComposer(QObject *parent = nullptr);

    bool keyEvent(Qt::Key key, const QString &text) override;
    bool selectCandidate(int index) override;
};
