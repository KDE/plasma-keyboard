/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "anthytextcomposer.h"
#include "textcomposertestutils.h"

#include <QTest>

class AnthyTextComposerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testCandidateSelection()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        // Type romaji
        QVERIFY(sendText(engine, u"romaji"));

        // Check that there's prediction candidates
        QCOMPARE(engine.preeditText(), QStringLiteral("ろまじ"));
        QVERIFY(engine.candidates().contains(QStringLiteral("ロマジ")));
        QVERIFY(engine.candidates().contains(QStringLiteral("ろまじ")));

        // Select the katakana prediction
        int katakanaIndex = engine.candidates().indexOf(QStringLiteral("ロマジ"));
        QVERIFY(engine.selectCandidate(katakanaIndex));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("ロマジ")});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testCandidateSelection2()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        // Type a prolonged vowel "-"
        QVERIFY(sendText(engine, u"ro-maji"));

        QCOMPARE(engine.preeditText(), QStringLiteral("ろーまじ"));
        QVERIFY(engine.candidates().contains(QStringLiteral("ローマジ")));
        QVERIFY(engine.candidates().contains(QStringLiteral("ローマ字")));
    }

    void testKanjiCandidateSelection()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        // Type nihongo
        QVERIFY(sendText(engine, u"nihongo"));

        // Select kanji (nihongo -> 日本語)
        QCOMPARE(engine.preeditText(), QStringLiteral("にほんご"));
        const int kanjiIndex = engine.candidates().indexOf(QStringLiteral("日本語"));
        QVERIFY(kanjiIndex >= 0);
        QVERIFY(engine.selectCandidate(kanjiIndex));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("日本語")});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testSpaceCyclesCandidates()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        QVERIFY(sendText(engine, u"nihongo"));
        QStringList candidates = engine.candidates();
        QVERIFY(candidates.size() > 1);

        // Test pressing space cycles candidates
        for (const QString &candidate : candidates) {
            QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, QStringLiteral(" ")));
            QCOMPARE(engine.preeditText(), candidate);
        }

        // Wraparound to beginning
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, QStringLiteral(" ")));
        QCOMPARE(engine.preeditText(), candidates.first());
        QVERIFY(backend->keyClickRequests.isEmpty());
    }

    void testKanaCyclingYa()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        // Pressing ya button should cycle (や -> ゃ -> や -> ...)

        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, QStringLiteral("ya")));
        QCOMPARE(engine.preeditText(), QStringLiteral("や"));

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("ゃ"));

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("や"));
    }

    // Test kana cycling for flick keyboard (pressing on a key repeatedly)
    void testKanaCycling_data()
    {
        QTest::addColumn<QString>("input");
        QTest::addColumn<QString>("unvoiced");
        QTest::addColumn<QString>("voiced");

        QTest::newRow("ka") << QStringLiteral("ka") << QStringLiteral("か") << QStringLiteral("が");
        QTest::newRow("sa") << QStringLiteral("sa") << QStringLiteral("さ") << QStringLiteral("ざ");
        QTest::newRow("shi") << QStringLiteral("shi") << QStringLiteral("し") << QStringLiteral("じ");
        QTest::newRow("chi") << QStringLiteral("chi") << QStringLiteral("ち") << QStringLiteral("ぢ");
    }

    void testKanaCycling()
    {
        QFETCH(QString, input);
        QFETCH(QString, unvoiced);
        QFETCH(QString, voiced);

        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        // Type unvoiced kana
        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, input));
        QCOMPARE(engine.preeditText(), unvoiced);

        // Add dakuten
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), voiced);

        // Remove dakuten
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), unvoiced);
    }

    void testKanaCyclingTsu()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        // tsu cycling is tricky, it needs to follow:
        // (つ -> っ -> づ -> つ -> ...)

        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, QStringLiteral("tsu")));
        QCOMPARE(engine.preeditText(), QStringLiteral("つ"));

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("っ"));

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("づ"));

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("つ"));
    }

    void testKanaCyclingHa()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        // ha cycling is tricky, it needs to follow:
        // (は -> ば -> ぱ -> は -> ...)

        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, QStringLiteral("ha")));
        QCOMPARE(engine.preeditText(), QStringLiteral("は"));

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("ば"));

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("ぱ"));

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("は"));
    }

    void testKanaCyclingInvalid()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        // な has no cycling, ensure it doesn't change when attempting to cycle it

        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, QStringLiteral("na")));
        QCOMPARE(engine.preeditText(), QStringLiteral("な"));

        QVERIFY(!composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("な"));
    }

    void testBackspaceAndReset()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        QVERIFY(sendText(engine, u"kana"));
        QCOMPARE(engine.preeditText(), QStringLiteral("かな"));

        // Delete "a" from formed "かな" should be "かn"
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), QStringLiteral("かn"));

        backend->reset();
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testKanaModeCapitalization()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        engine.setShiftActive(true);
        QVERIFY(sendText(engine, u"k"));
        QCOMPARE(engine.preeditText(), QStringLiteral("K"));

        engine.setShiftActive(false);
        QVERIFY(sendText(engine, u"a"));
        QCOMPARE(engine.preeditText(), QStringLiteral("か"));
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), QStringLiteral("K"));

        QVERIFY(engine.sendTextComposerKey(Qt::Key_Return, QString()));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("K")});
        QVERIFY(engine.preeditText().isEmpty());

        QVERIFY(sendText(engine, u"KaN"));
        QCOMPARE(engine.preeditText(), QStringLiteral("かN"));
        QVERIFY(composer->replaceLastInput(QStringLiteral("Y")));
        QCOMPARE(engine.preeditText(), QStringLiteral("かY"));
        backend->reset();
        QVERIFY(sendText(engine, u"n"));
        QCOMPARE(engine.preeditText(), QStringLiteral("n"));

        backend->reset();
        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, QStringLiteral("KA")));
        QCOMPARE(engine.preeditText(), QStringLiteral("か"));
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("が"));
    }

    void testLatinMode()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        composer->setInputMode(QStringLiteral("latin"));
        engine.setTextComposer(composer);

        QVERIFY(sendText(engine, u"a"));
        engine.setShiftActive(true);
        QVERIFY(sendText(engine, u"a"));

        // Test capitalization
        QCOMPARE(backend->commitRequests, (QStringList{QStringLiteral("a"), QStringLiteral("A")}));
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }
};

QTEST_GUILESS_MAIN(AnthyTextComposerTest)

#include "anthytextcomposertest.moc"
