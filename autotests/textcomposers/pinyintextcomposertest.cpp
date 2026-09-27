/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "pinyintextcomposer.h"
#include "emojicontroller.h"
#include "textcomposertestutils.h"

#include <QStandardPaths>
#include <QTest>

class PinyinTextComposerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        // Isolate user data
        QStandardPaths::setTestModeEnabled(true);
    }

    void convertsAndSelectsCandidate()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new PinyinTextComposer);

        // Type pinyin
        QVERIFY(sendText(engine, u"nihao"));

        // Select the hanzi candidate
        QCOMPARE(engine.preeditText(), QStringLiteral("nihao"));
        int candidateIndex = engine.candidates().indexOf(QStringLiteral("你好"));
        QVERIFY(candidateIndex >= 0);
        QVERIFY(engine.selectCandidate(candidateIndex));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("你好")});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void backspaceAndEscapeClearComposition()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new PinyinTextComposer);

        // Type pinyin
        QVERIFY(sendText(engine, u"ni"));

        // Delete the last input
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QCOMPARE(engine.preeditText(), QStringLiteral("n"));

        // Clear composition
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Escape, QString()));
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void latinModeCommitsDirectly()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new PinyinTextComposer;
        composer->setInputMode(QStringLiteral("latin"));
        engine.setTextComposer(composer);

        // Commit latin text
        QVERIFY(sendText(engine, u"a"));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("a")});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void emojiSearchUsesPinyinCandidates()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new PinyinTextComposer;
        engine.setTextComposer(composer);
        EmojiController controller(&engine);

        controller.open();
        controller.startSearch();
        QCOMPARE(engine.textComposer(), composer);

        QVERIFY(sendText(engine, u"nihao"));
        QCOMPARE(controller.query(), QStringLiteral("nihao"));
        QVERIFY(backend->preeditRequests.isEmpty());
        int candidateIndex = engine.candidates().indexOf(QStringLiteral("你好"));
        QVERIFY(candidateIndex >= 0);

        QVERIFY(engine.selectCandidate(candidateIndex));
        QCOMPARE(controller.query(), QStringLiteral("你好"));
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
        QVERIFY(backend->commitRequests.isEmpty());

        controller.commitEmoji(QStringLiteral("😀"));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("😀")});

        controller.stopSearch();
        QCOMPARE(engine.textComposer(), composer);
    }
};

QTEST_GUILESS_MAIN(PinyinTextComposerTest)

#include "pinyintextcomposertest.moc"
