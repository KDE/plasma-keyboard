/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "hangultextcomposer.h"
#include "textcomposertestutils.h"

#include <QTest>

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
        QCOMPARE(engine.preeditText(), QStringLiteral("한"));

        // Space should flush the full character 한
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, QStringLiteral(" ")));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("한")});
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
        QCOMPARE(engine.preeditText(), QStringLiteral("하"));

        backend->reset();
        QVERIFY(engine.preeditText().isEmpty());
    }

    void testShiftJamo()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new HangulTextComposer);

        QVERIFY(sendText(engine, u"ㅃㅏ"));
        QCOMPARE(engine.preeditText(), QStringLiteral("빠"));
    }

    void testLatin()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new HangulTextComposer;
        composer->setInputMode(QStringLiteral("latin"));
        engine.setTextComposer(composer);

        // Commit uppercase latin text
        QVERIFY(sendText(engine, u"a"));
        // composer->setTextCase(AbstractTextComposer::TextCase::Upper);
        engine.setShiftActive(true);
        QVERIFY(sendText(engine, u"a"));

        QCOMPARE(backend->commitRequests, (QStringList{QStringLiteral("a"), QStringLiteral("A")}));
        QVERIFY(engine.preeditText().isEmpty());
    }
};

QTEST_GUILESS_MAIN(HangulTextComposerTest)

#include "hangultextcomposertest.moc"
