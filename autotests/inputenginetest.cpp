/*
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "inputengine.h"
#include "basictextcomposer.h"
#include "directtextcomposer.h"
#include "inputbackend.h"

#include <QLoggingCategory>
#include <QSignalSpy>
#include <QTest>

Q_LOGGING_CATEGORY(PlasmaKeyboard, "org.kde.plasma.keyboard")

class TestInputBackend : public InputBackend
{
public:
    bool isActive() const override
    {
        return m_active;
    }

    Qt::InputMethodHints inputMethodHints() const override
    {
        return m_inputMethodHints;
    }

    QString surroundingText() const override
    {
        return m_surroundingText;
    }

    uint32_t cursorPositionUtf8() const override
    {
        return m_cursorPositionUtf8;
    }

    uint32_t anchorPositionUtf8() const override
    {
        return m_anchorPositionUtf8;
    }

    void setPreeditText(const QString &text) override
    {
        preeditRequests.append(text);
        events.append(QStringLiteral("preedit:%1").arg(text));
    }

    void commitText(const QString &text) override
    {
        commitRequests.append(text);
        events.append(QStringLiteral("commit:%1").arg(text));
    }

    void deleteSurroundingText(int index, int length) override
    {
        deleteRequests.append({index, length});
        events.append(QStringLiteral("delete:%1:%2").arg(index).arg(length));
    }

    bool sendKeyClick(int key) override
    {
        keyClickRequests.append(key);
        events.append(QStringLiteral("click:%1").arg(key));
        return keyClickHandled;
    }

    bool sendKeyPressed(int key, bool pressed) override
    {
        keyPressedRequests.append({key, pressed});
        events.append(QStringLiteral("key:%1:%2").arg(key).arg(pressed));
        if (keyPressedHandled) {
            setKeyPressed(key, pressed);
        }
        return keyPressedHandled;
    }

    void setActiveState(bool active)
    {
        if (m_active == active) {
            return;
        }
        m_active = active;
        Q_EMIT activeChanged();
    }

    void setInputMethodHintsState(Qt::InputMethodHints hints)
    {
        if (m_inputMethodHints == hints) {
            return;
        }
        m_inputMethodHints = hints;
        Q_EMIT inputMethodHintsChanged();
    }

    void setSurroundingState(const QString &text, uint32_t cursorPositionUtf8, uint32_t anchorPositionUtf8)
    {
        if (m_surroundingText == text && m_cursorPositionUtf8 == cursorPositionUtf8 && m_anchorPositionUtf8 == anchorPositionUtf8) {
            return;
        }
        m_surroundingText = text;
        m_cursorPositionUtf8 = cursorPositionUtf8;
        m_anchorPositionUtf8 = anchorPositionUtf8;
        Q_EMIT surroundingTextChanged();
    }

    void setSurroundingState(const QString &text, uint32_t cursorPositionUtf8)
    {
        setSurroundingState(text, cursorPositionUtf8, cursorPositionUtf8);
    }

    QStringList preeditRequests;
    QStringList commitRequests;
    QList<QPair<int, int>> deleteRequests;
    QList<int> keyClickRequests;
    QList<QPair<int, bool>> keyPressedRequests;
    QStringList events;
    bool keyClickHandled = true;
    bool keyPressedHandled = true;

private:
    bool m_active = false;
    // Keep automatic capitalization disabled unless a test opts in
    Qt::InputMethodHints m_inputMethodHints = Qt::ImhNoAutoUppercase;
    QString m_surroundingText;
    uint32_t m_cursorPositionUtf8 = 0;
    uint32_t m_anchorPositionUtf8 = 0;
};

class CandidateTextComposer : public DirectTextComposer
{
public:
    bool selectCandidate(int index) override
    {
        selectedCandidate = index;
        return true;
    }

    int selectedCandidate = -1;
};

class InputEngineTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    // Basic input

    void commitsExistingPreedit()
    {
        auto *backend = new TestInputBackend;
        InputEngine engine(backend);
        engine.setPreeditText(QStringLiteral("composed"));
        backend->events.clear();

        // An empty commit request accepts the current preedit
        engine.commit();

        QCOMPARE(backend->events, QStringList({QStringLiteral("commit:composed"), QStringLiteral("preedit:")}));
        QVERIFY(engine.preeditText().isEmpty());
    }

    void routesEditingKeysAsClicks()
    {
        auto *backend = new TestInputBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new DirectTextComposer);

        const QList<Qt::Key> keys = {
            Qt::Key_Backspace,
            Qt::Key_Delete,
            Qt::Key_Left,
            Qt::Key_Right,
            Qt::Key_Up,
            Qt::Key_Down,
            Qt::Key_Home,
            Qt::Key_End,
            Qt::Key_Return,
            Qt::Key_Enter,
            Qt::Key_Tab,
            Qt::Key_Space,
        };

        // Editing keys are forwarded as backend clicks instead of text
        for (Qt::Key key : keys) {
            QVERIFY(engine.sendTextComposerKey(key, QString()));
        }

        QCOMPARE(backend->keyClickRequests, QList<int>(keys.cbegin(), keys.cend()));
        QVERIFY(backend->commitRequests.isEmpty());
    }

    void routesComposerKeysDirectlyWithPressedModifier()
    {
        auto *backend = new TestInputBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new DirectTextComposer);

        // A held modifier routes composer keys directly to the backend
        QVERIFY(engine.sendDirectKey(Qt::Key_Control, true));
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Q, QStringLiteral("q")));

        QCOMPARE(backend->keyClickRequests, QList<int>{Qt::Key_Q});
        QVERIFY(backend->commitRequests.isEmpty());
        QCOMPARE(engine.pressedKeys(), QList<int>{Qt::Key_Control});
    }

    // Surrounding text

    void convertsUtf8PositionsAndSelections()
    {
        auto *backend = new TestInputBackend;
        InputEngine engine(backend);
        QString text = QStringLiteral("a😀éz");

        // UTF-8 byte offsets are converted to UTF-16 string positions
        backend->setSurroundingState(text, 5, 7);
        QCOMPARE(engine.cursorPosition(), 3);
        QCOMPARE(engine.anchorPosition(), 4);
        QCOMPARE(engine.selectedText(), QStringLiteral("é"));

        // Reverse selections preserve the selected text range
        backend->setSurroundingState(text, 7, 1);
        QCOMPARE(engine.cursorPosition(), 4);
        QCOMPARE(engine.anchorPosition(), 1);
        QCOMPARE(engine.selectedText(), QStringLiteral("😀é"));
    }

    // Composition

    void commitsReplacementAndClearsComposition()
    {
        auto *backend = new TestInputBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new DirectTextComposer);
        engine.setPreeditPrefix(QStringLiteral("prefix"));
        engine.setPreeditText(QStringLiteral("preedit"));
        engine.setCandidates({QStringLiteral("one"), QStringLiteral("two")});
        backend->events.clear();

        // Replacement deletes before committing and then clears composition
        engine.commit(QStringLiteral("replacement"), -2, 2);

        QCOMPARE(backend->events, QStringList({QStringLiteral("delete:-2:2"), QStringLiteral("commit:replacement"), QStringLiteral("preedit:")}));
        QVERIFY(engine.preeditPrefix().isEmpty());
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
        QVERIFY(!engine.wordCandidateListVisibleHint());
    }

    void candidateModelTracksAndSelectsCandidates()
    {
        auto *backend = new TestInputBackend;
        InputEngine engine(backend);
        auto *composer = new CandidateTextComposer;
        engine.setTextComposer(composer);
        WordCandidateListModel *model = engine.wordCandidateListModel();
        QSignalSpy countSpy(model, &WordCandidateListModel::countChanged);

        engine.setCandidates({QStringLiteral("first"), QStringLiteral("second")});

        // Candidate updates are exposed through the list model
        QCOMPARE(model->count(), 2);
        QCOMPARE(model->rowCount(), 2);
        QCOMPARE(model->data(model->index(0), WordCandidateListModel::DisplayRole).toString(), QStringLiteral("first"));
        QCOMPARE(model->data(model->index(1), Qt::DisplayRole).toString(), QStringLiteral("second"));
        QCOMPARE(countSpy.count(), 1);
        QVERIFY(engine.wordCandidateListVisibleHint());

        // Valid selections reach the composer and invalid ones are ignored
        model->selectItem(1);
        QCOMPARE(composer->selectedCandidate, 1);
        model->selectItem(2);
        QCOMPARE(composer->selectedCandidate, 1);

        // Clearing candidates updates model count and visibility
        engine.clear();
        QCOMPARE(model->count(), 0);
        QCOMPARE(countSpy.count(), 2);
        QVERIFY(!engine.wordCandidateListVisibleHint());
    }

    // Engine state

    void resetClearsTransientState()
    {
        auto *backend = new TestInputBackend;
        InputEngine engine(backend);
        engine.setTextComposer(new DirectTextComposer);
        backend->setActiveState(true);
        engine.setCapsLockActive(true);
        engine.setPreeditText(QStringLiteral("preedit"));
        engine.setCandidates({QStringLiteral("candidate")});
        QVERIFY(engine.sendDirectKey(Qt::Key_Control, true));

        // Backend reset clears transient state and releases held keys
        backend->reset();

        QVERIFY(!engine.capsLockActive());
        QVERIFY(!engine.shiftActive());
        QVERIFY(engine.preeditText().isEmpty());
        QVERIFY(engine.candidates().isEmpty());
        QVERIFY(engine.pressedKeys().isEmpty());
        QCOMPARE(backend->keyPressedRequests.last(), qMakePair(int(Qt::Key_Control), false));
    }

    // Casing and automatic capitalization

    void commitsComposerText()
    {
        auto *backend = new TestInputBackend;
        // Enable automatic capitalization to test one-shot shift
        backend->setInputMethodHintsState(Qt::ImhNone);
        InputEngine engine(backend);
        engine.setTextComposer(new BasicTextComposer);
        backend->setActiveState(true);

        // One-shot shift is consumed after committing text
        QVERIFY(engine.sendTextComposerKey(Qt::Key_Q, QStringLiteral("q")));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("Q")});
        QVERIFY(!engine.shiftActive());

        // Caps lock remains active after committing text
        engine.setCapsLockActive(true);
        QVERIFY(engine.sendTextComposerKey(Qt::Key_W, QStringLiteral("w")));
        QCOMPARE(backend->commitRequests, QStringList({QStringLiteral("Q"), QStringLiteral("W")}));
        QVERIFY(engine.capsLockActive());
    }

    void appliesLocaleAwareCasing()
    {
        auto *backend = new TestInputBackend;
        // Enable automatic capitalization to exercise locale-aware uppercasing
        backend->setInputMethodHintsState(Qt::ImhNone);
        InputEngine engine(backend);
        engine.setLocale(QStringLiteral("tr_TR"));
        engine.setTextComposer(new BasicTextComposer);
        backend->setActiveState(true);

        // Turkish dotted i follows locale-aware uppercasing
        QVERIFY(engine.sendTextComposerKey(Qt::Key_I, QStringLiteral("i")));
        QCOMPARE(backend->commitRequests, QStringList{QStringLiteral("İ")});
    }

    void automaticShiftFollowsContext_data()
    {
        QTest::addColumn<QString>("text");
        QTest::addColumn<uint32_t>("cursorPositionUtf8");
        QTest::addColumn<bool>("expected");

        QTest::newRow("empty-input") << QString() << 0u << true;
        QTest::newRow("middle-of-sentence") << QStringLiteral("Hello") << 5u << false;
        QTest::newRow("punctuation-without-space") << QStringLiteral("Hello.") << 6u << false;
        QTest::newRow("sentence-boundary") << QStringLiteral("Hello. ") << 7u << true;
        QTest::newRow("text-after-cursor") << QStringLiteral("First. second") << 7u << true;
        QTest::newRow("middle-after-boundary") << QStringLiteral("First. second") << 9u << false;
        QTest::newRow("utf-8-boundary") << QStringLiteral("é. ") << 4u << true;
    }

    void automaticShiftFollowsContext()
    {
        QFETCH(QString, text);
        QFETCH(uint32_t, cursorPositionUtf8);
        QFETCH(bool, expected);

        auto *backend = new TestInputBackend;
        // Enable automatic capitalization for these context checks
        backend->setInputMethodHintsState(Qt::ImhNone);
        backend->setSurroundingState(text, cursorPositionUtf8);
        InputEngine engine(backend);
        engine.setTextComposer(new BasicTextComposer);
        backend->setActiveState(true);

        QCOMPARE(engine.shiftActive(), expected);
    }

    void automaticShiftFollowsEngineState()
    {
        auto *backend = new TestInputBackend;
        // Enable automatic capitalization for this state transition test
        backend->setInputMethodHintsState(Qt::ImhNone);
        InputEngine engine(backend);
        backend->setActiveState(true);

        // Automatic shift requires a supporting composer
        QVERIFY(!engine.shiftActive());
        engine.setTextComposer(new BasicTextComposer);
        QVERIFY(engine.shiftActive());

        // Preedit and input hints can suppress automatic shift
        engine.setPreeditText(QStringLiteral("composition"));
        QVERIFY(!engine.shiftActive());
        engine.setPreeditText(QString());
        QVERIFY(engine.shiftActive());

        backend->setInputMethodHintsState(Qt::ImhNoAutoUppercase);
        QVERIFY(!engine.shiftActive());
        backend->setInputMethodHintsState(Qt::ImhNone);
        QVERIFY(engine.shiftActive());

        // Uppercase preference uses persistent caps lock
        backend->setInputMethodHintsState(Qt::ImhPreferUppercase);
        QVERIFY(engine.capsLockActive());
        QVERIFY(engine.uppercase());

        // Unsupported composers and inactive input clear uppercase state
        engine.setTextComposer(new DirectTextComposer);
        QVERIFY(!engine.shiftActive());
        QVERIFY(!engine.capsLockActive());

        engine.setTextComposer(new BasicTextComposer);
        backend->setInputMethodHintsState(Qt::ImhNone);
        backend->setActiveState(false);
        QVERIFY(!engine.shiftActive());
        QVERIFY(!engine.capsLockActive());
    }
};

QTEST_GUILESS_MAIN(InputEngineTest)

#include "inputenginetest.moc"
