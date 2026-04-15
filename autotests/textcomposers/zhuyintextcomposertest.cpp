/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "zhuyintextcomposer.h"
#include "textcomposertestutils.h"

#include <QStandardPaths>
#include <QTest>

class ZhuyinTextComposerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
    }

    void testBasic()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new ZhuyinTextComposer);

        // Type zhuyin
        QVERIFY(sendText(engine, u"ㄋㄧˇ"));

        // Select 你 candidate
        int candidateIndex = engine.candidates().indexOf(QStringLiteral("你"));
        QVERIFY(candidateIndex >= 0);
        QVERIFY(engine.selectCandidate(candidateIndex));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("你")});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testBackspace()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new ZhuyinTextComposer);

        QVERIFY(sendText(engine, u"ㄋㄧ"));
        QVERIFY(!engine.preeditText().isEmpty());

        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QVERIFY(!engine.preeditText().isEmpty());

        backend->reset();
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testLatinMode()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new ZhuyinTextComposer;
        composer->setInputMode(QStringLiteral("latin"));
        engine.setTextComposer(composer);

        QVERIFY(sendText(engine, u"a"));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("a")});
        QVERIFY(engine.preeditText().isEmpty());
    }
};

QTEST_GUILESS_MAIN(ZhuyinTextComposerTest)

#include "zhuyintextcomposertest.moc"
