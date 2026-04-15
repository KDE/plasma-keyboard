/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "pinyintextcomposer.h"
#include "textcomposertestutils.h"

#include <QStandardPaths>
#include <QTest>

class PinyinTextComposerTest : public QObject
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
        engine.setTextComposer(new PinyinTextComposer);

        // Type pinyin
        QVERIFY(sendText(engine, u"nihao"));

        // Select 你好 candidate
        QCOMPARE(engine.preeditText(), QStringLiteral("nihao"));
        int candidateIndex = engine.candidates().indexOf(QStringLiteral("你好"));
        QVERIFY(candidateIndex >= 0);
        QVERIFY(engine.selectCandidate(candidateIndex));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("你好")});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testBackspace()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new PinyinTextComposer);

        QVERIFY(sendText(engine, u"ni"));

        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), QStringLiteral("n"));

        QVERIFY(engine.sendTextComposerKey(Qt::Key_Escape, QString()));
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testLatinMode()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new PinyinTextComposer;
        composer->setInputMode(QStringLiteral("latin"));
        engine.setTextComposer(composer);

        QVERIFY(sendText(engine, u"a"));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("a")});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }
};

QTEST_GUILESS_MAIN(PinyinTextComposerTest)

#include "pinyintextcomposertest.moc"
