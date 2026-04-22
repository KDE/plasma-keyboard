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
    void composesAndFlushesHangul()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new HangulTextComposer);

        // Compose hangul
        QVERIFY(sendText(engine, u"ㅎㅏㄴ"));
        QCOMPARE(engine.preeditText(), QStringLiteral("한"));

        // Flush with space
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, QStringLiteral(" ")));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("한")});
        QCOMPARE(backend->keyClickRequests, QList<int>{Qt::Key_Space});
        QVERIFY(engine.preeditText().isEmpty());
    }

    void backspaceEditsComposition()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new HangulTextComposer);

        // Compose hangul
        QVERIFY(sendText(engine, u"ㅎㅏㄴ"));

        // Delete the final jamo
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), QStringLiteral("하"));

        // Reset composition
        backend->reset();
        QVERIFY(engine.preeditText().isEmpty());
    }

    void composesShiftedJamo()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new HangulTextComposer);

        // Compose shifted jamo
        QVERIFY(sendText(engine, u"ㅃㅏ"));
        QCOMPARE(engine.preeditText(), QStringLiteral("빠"));
    }

    void latinModeAppliesTextCase()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new HangulTextComposer;
        composer->setInputMode(QStringLiteral("latin"));
        composer->setTextCase(AbstractTextComposer::TextCase::Upper);
        engine.setTextComposer(composer);
        engine.setShiftActive(true);

        // Commit uppercase latin text
        QVERIFY(sendText(engine, u"a"));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("A")});
        QVERIFY(engine.preeditText().isEmpty());
    }
};

QTEST_GUILESS_MAIN(HangulTextComposerTest)

#include "hangultextcomposertest.moc"
