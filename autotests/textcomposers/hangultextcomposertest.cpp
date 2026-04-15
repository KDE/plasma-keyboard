/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "hangul/hangultextcomposer.h"
#include "textcomposertestutils.h"

#include <QTest>

using namespace Qt::StringLiterals;

class HangulTextComposerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testBasicCompose()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new HangulTextComposer);

        // Compose hangul components ㅎㅏㄴ -> 한
        QVERIFY(sendText(engine, u"ㅎㅏㄴ"));
        QCOMPARE(engine.preeditText(), u"한"_s);

        // Space should flush the full character 한
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, u" "_s));
        QCOMPARE(backend->commitRequests, QStringList{u"한"_s});
        QCOMPARE(backend->keyClickRequests, QList<int>{Qt::Key_Space});
        QVERIFY(engine.preeditText().isEmpty());
    }

    void testBackspace()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new HangulTextComposer);

        // Compose hangul
        QVERIFY(sendText(engine, u"ㅎㅏㄴ"));

        // Backspace deletes ㄴ from 한
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), u"하"_s);

        backend->reset();
        QVERIFY(engine.preeditText().isEmpty());
    }

    void testShiftJamo()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new HangulTextComposer);

        QVERIFY(sendText(engine, u"ㅃㅏ"));
        QCOMPARE(engine.preeditText(), u"빠"_s);
    }

    void testLatin()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new HangulTextComposer;
        composer->setInputMode(u"latin"_s);
        engine.setTextComposer(composer);

        // Commit uppercase latin text
        QVERIFY(sendText(engine, u"a"));
        // composer->setTextCase(AbstractTextComposer::TextCase::Upper);
        engine.setShiftActive(true);
        QVERIFY(sendText(engine, u"a"));

        QCOMPARE(backend->commitRequests, (QStringList{u"a"_s, u"A"_s}));
        QVERIFY(engine.preeditText().isEmpty());
    }
};

QTEST_GUILESS_MAIN(HangulTextComposerTest)

#include "hangultextcomposertest.moc"
