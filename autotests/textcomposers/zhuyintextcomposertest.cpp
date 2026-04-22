/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "zhuyintextcomposer.h"
#include "textcomposertestutils.h"

#include <QStandardPaths>
#include <QTest>

class ZhuyinTextComposerTest : public QObject
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
        engine.setTextComposer(new ZhuyinTextComposer);

        // Type zhuyin
        QVERIFY(sendText(engine, u"ㄋㄧˇ"));

        // Select the hanzi candidate
        int candidateIndex = engine.candidates().indexOf(QStringLiteral("你"));
        QVERIFY(candidateIndex >= 0);
        QVERIFY(engine.selectCandidate(candidateIndex));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("你")});
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void backspaceAndResetClearComposition()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new ZhuyinTextComposer);

        // Type zhuyin
        QVERIFY(sendText(engine, u"ㄋㄧ"));
        QVERIFY(!engine.preeditText().isEmpty());

        // Delete the last input
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, QString()));
        QVERIFY(!engine.preeditText().isEmpty());

        // Reset composition
        backend->reset();
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
    }

    void latinModeCommitsDirectly()
    {
        auto *backend = new TextComposerTestBackend;
        InputEngine engine(backend);
        auto *composer = new ZhuyinTextComposer;
        composer->setInputMode(QStringLiteral("latin"));
        engine.setTextComposer(composer);

        // Commit latin text
        QVERIFY(sendText(engine, u"a"));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("a")});
        QVERIFY(engine.preeditText().isEmpty());
    }
};

QTEST_GUILESS_MAIN(ZhuyinTextComposerTest)

#include "zhuyintextcomposertest.moc"
