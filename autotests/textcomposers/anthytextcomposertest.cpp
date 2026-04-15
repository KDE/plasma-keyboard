/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "anthy/anthytextcomposer.h"
#include "textcomposertestutils.h"

#include <QFile>
#include <QTemporaryDir>
#include <QTest>

#include <anthy/anthy.h>

using namespace Qt::StringLiterals;

class AnthyTextComposerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        // Use temporary folder
        QVERIFY(m_home.isValid());
        anthy_conf_override("HOME", QFile::encodeName(m_home.path()).constData());
    }

    void testCandidateSelection()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        // Type romaji
        QVERIFY(sendText(engine, u"romaji"));

        // Check that there's prediction candidates
        QCOMPARE(engine.preeditText(), u"ろまじ"_s);
        QVERIFY(engine.candidates().contains(u"ロマジ"_s));
        QVERIFY(engine.candidates().contains(u"ろまじ"_s));

        // Select the katakana prediction
        int katakanaIndex = engine.candidates().indexOf(u"ロマジ"_s);
        QVERIFY(engine.selectCandidate(katakanaIndex));
        QCOMPARE(backend->commitRequests, QStringList{u"ロマジ"_s});
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

        QCOMPARE(engine.preeditText(), u"ろーまじ"_s);
        QVERIFY(engine.candidates().contains(u"ローマジ"_s));
        QVERIFY(engine.candidates().contains(u"ローマ字"_s));
    }

    void testKanjiCandidateSelection()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        // Type nihongo
        QVERIFY(sendText(engine, u"nihongo"));

        // Select kanji (nihongo -> 日本語)
        QCOMPARE(engine.preeditText(), u"にほんご"_s);
        const int kanjiIndex = engine.candidates().indexOf(u"日本語"_s);
        QVERIFY(kanjiIndex >= 0);
        QVERIFY(engine.selectCandidate(kanjiIndex));
        QCOMPARE(backend->commitRequests, QStringList{u"日本語"_s});
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
            QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, u" "_s));
            QCOMPARE(engine.preeditText(), candidate);
        }

        // Wraparound to beginning
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, u" "_s));
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

        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, u"ya"_s));
        QCOMPARE(engine.preeditText(), u"や"_s);

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), u"ゃ"_s);

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), u"や"_s);
    }

    // Test kana cycling for flick keyboard (pressing on a key repeatedly)
    void testKanaCycling_data()
    {
        QTest::addColumn<QString>("input");
        QTest::addColumn<QString>("unvoiced");
        QTest::addColumn<QString>("voiced");

        QTest::newRow("ka") << u"ka"_s << u"か"_s << u"が"_s;
        QTest::newRow("sa") << u"sa"_s << u"さ"_s << u"ざ"_s;
        QTest::newRow("shi") << u"shi"_s << u"し"_s << u"じ"_s;
        QTest::newRow("chi") << u"chi"_s << u"ち"_s << u"ぢ"_s;
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

        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, u"tsu"_s));
        QCOMPARE(engine.preeditText(), u"つ"_s);

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), u"っ"_s);

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), u"づ"_s);

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), u"つ"_s);
    }

    void testKanaCyclingHa()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        // ha cycling is tricky, it needs to follow:
        // (は -> ば -> ぱ -> は -> ...)

        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, u"ha"_s));
        QCOMPARE(engine.preeditText(), u"は"_s);

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), u"ば"_s);

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), u"ぱ"_s);

        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), u"は"_s);
    }

    void testKanaCyclingInvalid()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        // な has no cycling, ensure it doesn't change when attempting to cycle it

        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, u"na"_s));
        QCOMPARE(engine.preeditText(), u"な"_s);

        QVERIFY(!composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), u"な"_s);
    }

    void testBackspaceAndReset()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        QVERIFY(sendText(engine, u"kana"));
        QCOMPARE(engine.preeditText(), u"かな"_s);

        // Delete "a" from formed "かな" should be "かn"
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), u"かn"_s);

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

        // Shift capitalizes romaji, which stays as typed until it spells a kana
        engine.setShiftActive(true);
        QVERIFY(sendText(engine, u"k"));
        QCOMPARE(engine.preeditText(), u"K"_s);
        engine.setShiftActive(false);
        QVERIFY(sendText(engine, u"a"));
        QCOMPARE(engine.preeditText(), u"か"_s);

        // Deleting the kana brings the romaji back as typed
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), u"K"_s);
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Return, QString()));
        QCOMPARE(backend->commitRequests, QStringList{u"K"_s});

        // Capitalized romaji works with the kana modifier key
        QVERIFY(engine.sendTextComposerKey(Qt::Key_unknown, u"KA"_s));
        QCOMPARE(engine.preeditText(), u"か"_s);
        QVERIFY(composer->cycleKanaModifier());
        QCOMPARE(engine.preeditText(), u"が"_s);
    }

    void testLatinMode()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        composer->setInputMode(u"latin"_s);
        engine.setTextComposer(composer);

        QVERIFY(sendText(engine, u"a"));
        engine.setShiftActive(true);
        QVERIFY(sendText(engine, u"a"));

        // Test capitalization
        QCOMPARE(backend->commitRequests, (QStringList{u"a"_s, u"A"_s}));
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void testSpelling_data()
    {
        QTest::addColumn<QString>("input");
        QTest::addColumn<QString>("preedit");

        QTest::newRow("pending romaji") << u"kanak"_s << u"かなk"_s;
        QTest::newRow("n apostrophe") << u"kan'i"_s << u"かんい"_s;
        QTest::newRow("punctuation") << u"a-a~a,a."_s << u"あーあ〜あ、あ。"_s;
        QTest::newRow("vu") << u"vu"_s << u"ゔ"_s;
        // Letters that don't spell kana are kept as typed, rather than dropped
        QTest::newRow("unspellable letter") << u"Kqa"_s << u"Kqあ"_s;
        QTest::newRow("unspellable letter after n") << u"honq"_s << u"ほんq"_s;
        QTest::newRow("unspellable before punctuation") << u"ab."_s << u"あb。"_s;
    }

    void testSpelling()
    {
        QFETCH(QString, input);
        QFETCH(QString, preedit);

        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        QVERIFY(sendText(engine, input));
        QCOMPARE(engine.preeditText(), preedit);
    }

    void testUnfinishedRomaji()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        // A trailing n is ん once committed
        QVERIFY(sendText(engine, u"hon"));
        QCOMPARE(engine.preeditText(), u"ほn"_s);
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Return, QString()));
        QCOMPARE(backend->commitRequests.last(), u"ほん"_s);

        // Other unfinished romaji can't be converted, so it stays after the conversion
        QVERIFY(sendText(engine, u"nihongok"));
        QCOMPARE(engine.preeditText(), u"にほんごk"_s);
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, u" "_s));
        QCOMPARE(engine.preeditText(), u"日本語k"_s);
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Return, QString()));
        QCOMPARE(backend->commitRequests.last(), u"日本語k"_s);
    }

    void testCancelConversion()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new AnthyTextComposer);

        QVERIFY(sendText(engine, u"nihongo"));
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, u" "_s));
        QCOMPARE(engine.preeditText(), u"日本語"_s);

        // Backspace returns to the reading, then deletes from it
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), u"にほんご"_s);
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), u"にほんg"_s);

        // Escape returns to the reading, then discards it
        QVERIFY(sendText(engine, u"o"));
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, u" "_s));
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Escape, QString()));
        QCOMPARE(engine.preeditText(), u"にほんご"_s);
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Escape, QString()));
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(backend->commitRequests.isEmpty());
    }

    void testModeSwitchCommits()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new AnthyTextComposer;
        engine.setTextComposer(composer);

        QVERIFY(sendText(engine, u"kana"));
        composer->setInputMode(u"latin"_s);
        QCOMPARE(backend->commitRequests, QStringList{u"かな"_s});
        QVERIFY(engine.preeditText().isEmpty());
    }

private:
    QTemporaryDir m_home;
};

QTEST_GUILESS_MAIN(AnthyTextComposerTest)

#include "anthytextcomposertest.moc"
