/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "pinyin/pinyintextcomposer.h"
#include "textcomposertestutils.h"

#include <QStandardPaths>
#include <QTest>

using namespace Qt::StringLiterals;

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
        QCOMPARE(engine.preeditText(), u"nihao"_s);
        int candidateIndex = engine.candidates().indexOf(u"你好"_s);
        QVERIFY(candidateIndex >= 0);
        QVERIFY(engine.selectCandidate(candidateIndex));
        QCOMPARE(backend->commitRequests, QStringList{u"你好"_s});
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
        QCOMPARE(engine.preeditText(), u"n"_s);

        QVERIFY(engine.sendTextComposerKey(Qt::Key_Escape, QString()));
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testPartialSelection()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new PinyinTextComposer);

        // Matching 你 (ni) for "nihao" should leave "hao" in the preedit
        QVERIFY(sendText(engine, u"nihao"));
        QVERIFY(engine.selectCandidate(engine.candidates().indexOf(u"你"_s)));
        QCOMPARE(backend->commitRequests, QStringList{u"你"_s});
        QCOMPARE(engine.preeditText(), u"hao"_s);

        QVERIFY(engine.selectCandidate(engine.candidates().indexOf(u"好"_s)));
        QCOMPARE(backend->commitRequests, (QStringList{u"你"_s, u"好"_s}));
        QVERIFY(engine.preeditText().isEmpty());
    }

    void testSwitchInputMode()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new PinyinTextComposer;
        engine.setTextComposer(composer);

        QVERIFY(sendText(engine, u"nihao"));
        composer->setInputMode(u"latin"_s);
        QCOMPARE(backend->commitRequests, QStringList{u"你好"_s});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testLatinMode()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new PinyinTextComposer;
        composer->setInputMode(u"latin"_s);
        engine.setTextComposer(composer);

        QVERIFY(sendText(engine, u"a"));
        engine.setShiftActive(true);
        QVERIFY(sendText(engine, u"a"));
        engine.setCapsLockActive(true);
        QVERIFY(sendText(engine, u"a"));
        QCOMPARE(backend->commitRequests, (QStringList{u"a"_s, u"A"_s, u"A"_s}));
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }
};

QTEST_GUILESS_MAIN(PinyinTextComposerTest)

#include "pinyintextcomposertest.moc"
