/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "zhuyin/zhuyintextcomposer.h"
#include "textcomposertestutils.h"

#include <QStandardPaths>
#include <QTest>

using namespace Qt::StringLiterals;

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
        int candidateIndex = engine.candidates().indexOf(u"你"_s);
        QVERIFY(candidateIndex >= 0);
        QVERIFY(engine.selectCandidate(candidateIndex));
        QCOMPARE(backend->commitRequests, QStringList{u"你"_s});
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

    void testCommitIncomplete()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new ZhuyinTextComposer);

        // Committing (pressing "enter") should still preserve the preedit
        QVERIFY(sendText(engine, u"ㄋㄧˇㄏㄠˇㄋㄧ"));
        QCOMPARE(engine.preeditText(), u"你好ㄋㄧ"_s);
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Return, QString()));
        QCOMPARE(backend->commitRequests, QStringList{u"你好ㄋㄧ"_s});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());

        // A syllable without a tone is committed as typed, even if it would be valid with the first tone
        // ㄋㄧˇ = ni3 = 你, ㄇㄚ = ma (no tone yet)
        backend->commitRequests.clear();
        QVERIFY(sendText(engine, u"ㄋㄧˇㄇㄚ"));
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Return, QString()));
        QCOMPARE(backend->commitRequests, QStringList{u"你ㄇㄚ"_s});
    }

    void testSpaceFirstTone()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new ZhuyinTextComposer);

        // Space completes the syllable with the first tone but keeps composing
        // ㄓㄨㄥ + space = zhong1 = 中
        QVERIFY(sendText(engine, u"ㄓㄨㄥ"));
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, u" "_s));
        QCOMPARE(engine.preeditText(), u"中"_s);
        QVERIFY(backend->commitRequests.isEmpty());

        // ㄨㄣˊ = wen2 = 文
        QVERIFY(sendText(engine, u"ㄨㄣˊ"));
        QCOMPARE(engine.preeditText(), u"中文"_s);
        QVERIFY(backend->commitRequests.isEmpty());

        // With no syllable waiting for a tone, space commits
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Space, u" "_s));
        QCOMPARE(backend->commitRequests, QStringList{u"中文"_s});
    }

    void testBackspaceCandidates()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new ZhuyinTextComposer);

        // ㄋㄧˇ = ni3 = 你, ㄏㄠˇ = hao3 = 好
        QVERIFY(sendText(engine, u"ㄋㄧˇㄏㄠˇ"));

        // Backspace deletes the whole character 好
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), u"你"_s);

        // The candidates for ㄧ (yi) should not be affected by the deleted 好
        QVERIFY(sendText(engine, u"ㄧ"));
        int candidateIndex = engine.candidates().indexOf(u"衣"_s);
        QVERIFY(candidateIndex >= 0);
        QVERIFY(engine.selectCandidate(candidateIndex));
        QCOMPARE(backend->commitRequests, QStringList{u"你衣"_s});
    }

    void testBackspaceToEmpty()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new ZhuyinTextComposer);

        // ㄋㄧˇ = ni3 = 你, then ㄕ starts another syllable
        QVERIFY(sendText(engine, u"ㄋㄧˇㄕ"));

        // Delete ㄕ, then 你
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QVERIFY(engine.preeditText().isEmpty());

        // The candidates for ㄇㄚ (ma) should not be affected by the deleted input
        QVERIFY(sendText(engine, u"ㄇㄚ"));
        int candidateIndex = engine.candidates().indexOf(u"媽"_s);
        QVERIFY(candidateIndex >= 0);
        QVERIFY(engine.selectCandidate(candidateIndex));
        QCOMPARE(backend->commitRequests, QStringList{u"媽"_s});
    }

    void testSelectEveryCandidate()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new ZhuyinTextComposer);

        // ㄕˋ = shi4 has many candidates (是, 事, 市...)
        QVERIFY(sendText(engine, u"ㄕˋ"));
        const QStringList candidates = engine.candidates();
        QVERIFY(candidates.size() > 1);
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Escape, QString()));

        // Selecting each candidate should commit exactly that candidate
        for (int i = 0; i < candidates.size(); ++i) {
            backend->commitRequests.clear();
            QVERIFY(sendText(engine, u"ㄕˋ"));
            QVERIFY(engine.selectCandidate(i));
            QCOMPARE(backend->commitRequests, QStringList{candidates.at(i)});
        }
    }

    void testLatinMode()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new ZhuyinTextComposer;
        engine.setTextComposer(composer);

        // Switching to Latin commits unfinished preedit before Latin text
        QVERIFY(sendText(engine, u"ㄋㄧ"));
        composer->setInputMode(u"latin"_s);
        QVERIFY(sendText(engine, u"a"));
        QCOMPARE(backend->commitRequests, (QStringList{u"ㄋㄧ"_s, u"a"_s}));
        QVERIFY(engine.preeditText().isEmpty());
    }
};

QTEST_GUILESS_MAIN(ZhuyinTextComposerTest)

#include "zhuyintextcomposertest.moc"
