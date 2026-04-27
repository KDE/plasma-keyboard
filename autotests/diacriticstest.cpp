// SPDX-FileCopyrightText: 2026 Aleix Pol <aleixpol@kde.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "plasmakeyboardtestcase.h"

#include <KConfig>
#include <KConfigGroup>

#include <QSignalSpy>
#include <QtTest/QTest>

#include <linux/input-event-codes.h>

using namespace Qt::StringLiterals;

class DiacriticsTest : public PlasmaKeyboardTestCase
{
    Q_OBJECT

protected:
    void setupConfig() override
    {
        KConfig cfg(QStringLiteral("plasmakeyboardrc"));
        KConfigGroup grp(&cfg, QStringLiteral("General"));
        grp.writeEntry(QStringLiteral("enabledLocales"), QStringLiteral("it_IT"));
    }

private Q_SLOTS:
    void testLongPressShowsOverlayPanel()
    {
        QSignalSpy overlaySpy(m_inputPanel.get(), &InputPanelV1::overlayPanelRequested);

        sendKey(KEY_A, 1200);
        QVERIFY(overlaySpy.count() || overlaySpy.wait());

        QSignalSpy commitStringSpy(m_inputMethod->context(), &InputMethodContext::commitStringChanged);
        sendKey(KEY_1, 10);
        QVERIFY(commitStringSpy.count() || commitStringSpy.wait());
        QCOMPARE(commitStringSpy.count(), 1);
        QCOMPARE(commitStringSpy.first().first().toString(), QStringLiteral("á"));
    }
};

QTEST_MAIN(DiacriticsTest)

#include "diacriticstest.moc"
