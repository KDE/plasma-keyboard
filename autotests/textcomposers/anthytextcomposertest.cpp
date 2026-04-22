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
    void candidatesCoverWholeReading()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        // Type romaji
        QVERIFY(sendText(engine, u"romaji"));

        // Check full reading candidates
        QCOMPARE(engine.preeditText(), QStringLiteral("ろまじ"));
        QVERIFY(engine.candidates().contains(QStringLiteral("ロマジ")));
        QVERIFY(engine.candidates().contains(QStringLiteral("ろまじ")));
        QVERIFY(!engine.candidates().contains(QStringLiteral("炉")));
        QVERIFY(!engine.candidates().contains(QStringLiteral("ロ")));

        // Select katakana
        int katakanaIndex = engine.candidates().indexOf(QStringLiteral("ロマジ"));
        QVERIFY(engine.selectCandidate(katakanaIndex));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("ロマジ")});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void acceptsProlongedSoundMark()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        // Type a prolonged vowel
        QVERIFY(sendText(engine, u"ro-maji"));

        // Check prolonged sound candidates
        QCOMPARE(engine.preeditText(), QStringLiteral("ろーまじ"));
        QVERIFY(engine.candidates().contains(QStringLiteral("ローマジ")));
        QVERIFY(engine.candidates().contains(QStringLiteral("ローマ字")));
    }

    void convertsKanjiCandidate()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        // Type nihongo
        QVERIFY(sendText(engine, u"nihongo"));

        // Select the kanji candidate
        QCOMPARE(engine.preeditText(), QStringLiteral("にほんご"));
        const int kanjiIndex = engine.candidates().indexOf(QStringLiteral("日本語"));
        QVERIFY(kanjiIndex >= 0);
        QVERIFY(engine.selectCandidate(kanjiIndex));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("日本語")});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void spaceCyclesCandidates()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        // Build candidates for nihongo
        QVERIFY(sendText(engine, u"nihongo"));
        QStringList candidates = engine.candidates();
        QVERIFY(candidates.size() > 1);

        // Select each candidate with space
        for (const QString &candidate : candidates) {
            QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, QStringLiteral(" ")));
            QCOMPARE(engine.preeditText(), candidate);
        }

        // Wrap to the first candidate
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, QStringLiteral(" ")));
        QCOMPARE(engine.preeditText(), candidates.first());
        QVERIFY(backend->keyClickRequests.isEmpty());
    }

    void togglesSmallKana()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        // Type large ya
        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, QStringLiteral("ya")));
        QCOMPARE(engine.preeditText(), QStringLiteral("や"));

        // Make ya small
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("ゃ"));

        // Make ya large
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("や"));
    }

    void togglesDakuten_data()
    {
        QTest::addColumn<QString>("input");
        QTest::addColumn<QString>("unvoiced");
        QTest::addColumn<QString>("voiced");

        QTest::newRow("ka") << QStringLiteral("ka") << QStringLiteral("か") << QStringLiteral("が");
        QTest::newRow("sa") << QStringLiteral("sa") << QStringLiteral("さ") << QStringLiteral("ざ");
        QTest::newRow("shi") << QStringLiteral("shi") << QStringLiteral("し") << QStringLiteral("じ");
        QTest::newRow("chi") << QStringLiteral("chi") << QStringLiteral("ち") << QStringLiteral("ぢ");
    }

    void togglesDakuten()
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

    void cyclesTsuSmallAndDakuten()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        // Type large tsu
        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, QStringLiteral("tsu")));
        QCOMPARE(engine.preeditText(), QStringLiteral("つ"));

        // Make tsu small
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("っ"));

        // Replace small tsu with dakuten tsu
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("づ"));

        // Remove dakuten
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("つ"));
    }

    void cyclesHaDakutenAndHandakuten()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        // Type unvoiced ha
        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, QStringLiteral("ha")));
        QCOMPARE(engine.preeditText(), QStringLiteral("は"));

        // Add dakuten
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("ば"));

        // Replace dakuten with handakuten
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("ぱ"));

        // Remove handakuten
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("は"));
    }

    void ignoresUnsupportedKanaModifier()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        // Type kana without a modifier cycle
        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, QStringLiteral("na")));
        QCOMPARE(engine.preeditText(), QStringLiteral("な"));

        // Leave unsupported kana unchanged
        QVERIFY(!composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), QStringLiteral("な"));
    }

    void backspaceAndResetClearComposition()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        // Type kana
        QVERIFY(sendText(engine, u"kana"));
        QCOMPARE(engine.preeditText(), QStringLiteral("かな"));

        // Delete the last input
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), QStringLiteral("かn"));

        // Reset composition
        backend->reset();
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }
};

QTEST_GUILESS_MAIN(AnthyTextComposerTest)

#include "anthytextcomposertest.moc"
