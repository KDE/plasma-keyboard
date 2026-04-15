// SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "inputbackend.h"
#include "inputengine.h"
#include "textcomposers/directtextcomposer.h"

#include <QLoggingCategory>
#include <QTest>

Q_LOGGING_CATEGORY(PlasmaKeyboard, "org.kde.plasma.keyboard", QtWarningMsg)

class FakeInputBackend : public InputBackend
{
public:
    using InputBackend::InputBackend;

    bool isActive() const override
    {
        return true;
    }

    Qt::InputMethodHints inputMethodHints() const override
    {
        return {};
    }

    QString surroundingText() const override
    {
        return surrounding;
    }

    uint32_t cursorPositionUtf8() const override
    {
        return cursorPosition;
    }

    uint32_t anchorPositionUtf8() const override
    {
        return anchorPosition;
    }

    void setPreeditText(const QString &text) override
    {
        preeditText = text;
    }

    void commitText(const QString &text) override
    {
        committedText = text;
    }

    void deleteSurroundingText(int index, int length) override
    {
        deletedFrom = index;
        deletedLength = length;
    }

    bool sendKeyClick(int key) override
    {
        clickedKeys.append(key);
        return true;
    }

    bool sendKeyPressed(int key, bool pressed) override
    {
        setKeyPressed(key, pressed);
        return true;
    }

    QString preeditText;
    QString committedText;
    QString surrounding;
    uint32_t cursorPosition = 0;
    uint32_t anchorPosition = 0;
    int deletedFrom = 0;
    int deletedLength = 0;
    QList<int> clickedKeys;
};

class VirtualKeyboardTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void commitsText();
    void commitsComposition();
    void replacesText();
    void routesSpecialKeys();
    void modifierBypassesComposer();
    void readsUnicodeSelection();
};

void VirtualKeyboardTest::commitsText()
{
    FakeInputBackend backend(this);
    InputEngine engine(&backend);
    DirectTextComposer composer;
    engine.setTextComposer(&composer);
    engine.setLocale(QStringLiteral("en_US"));
    engine.setShiftActive(true);

    QVERIFY(engine.sendTextComposerKey(Qt::Key_A, QStringLiteral("a")));
    QCOMPARE(backend.committedText, QStringLiteral("A"));
    QVERIFY(!engine.shiftActive());
}

void VirtualKeyboardTest::commitsComposition()
{
    FakeInputBackend backend(this);
    InputEngine engine(&backend);
    auto *candidateModel = engine.wordCandidateListModel();
    engine.setPreeditText(QStringLiteral("hello"));
    engine.setPreeditPrefix(QStringLiteral("he"));
    engine.setCandidates({QStringLiteral("hello")});

    QCOMPARE(candidateModel->rowCount(), 1);
    engine.commit();

    QCOMPARE(backend.committedText, QStringLiteral("hello"));
    QVERIFY(backend.preeditText.isEmpty());
    QVERIFY(engine.preeditText().isEmpty());
    QVERIFY(engine.preeditPrefix().isEmpty());
    QVERIFY(engine.candidates().isEmpty());
    QCOMPARE(candidateModel->rowCount(), 0);
}

void VirtualKeyboardTest::replacesText()
{
    FakeInputBackend backend(this);
    InputEngine engine(&backend);
    engine.setPreeditText(QStringLiteral("old"));

    engine.commit(QStringLiteral("new"), -3, 3);

    QCOMPARE(backend.deletedFrom, -3);
    QCOMPARE(backend.deletedLength, 3);
    QCOMPARE(backend.committedText, QStringLiteral("new"));
    QVERIFY(engine.preeditText().isEmpty());
}

void VirtualKeyboardTest::routesSpecialKeys()
{
    FakeInputBackend backend(this);
    InputEngine engine(&backend);
    DirectTextComposer composer;
    engine.setTextComposer(&composer);

    QVERIFY(engine.sendTextComposerKey(Qt::Key_Backspace, {}));
    QCOMPARE(backend.clickedKeys, QList<int>{Qt::Key_Backspace});
    QVERIFY(backend.committedText.isEmpty());
}

void VirtualKeyboardTest::modifierBypassesComposer()
{
    FakeInputBackend backend(this);
    InputEngine engine(&backend);
    DirectTextComposer composer;
    engine.setTextComposer(&composer);

    QVERIFY(engine.sendDirectKey(Qt::Key_Control, true));
    QVERIFY(engine.isKeyPressed(Qt::Key_Control));
    QVERIFY(engine.sendTextComposerKey(Qt::Key_A, QStringLiteral("a")));
    QCOMPARE(backend.clickedKeys, QList<int>{Qt::Key_A});
    QVERIFY(backend.committedText.isEmpty());
    QVERIFY(engine.sendDirectKey(Qt::Key_Control, false));
    QVERIFY(!engine.isKeyPressed(Qt::Key_Control));
}

void VirtualKeyboardTest::readsUnicodeSelection()
{
    FakeInputBackend backend(this);
    backend.surrounding = QStringLiteral("aé𐐷z");
    backend.cursorPosition = 3;
    backend.anchorPosition = 7;
    InputEngine engine(&backend);

    QCOMPARE(engine.cursorPosition(), 2);
    QCOMPARE(engine.anchorPosition(), 4);
    QCOMPARE(engine.selectedText(), QStringLiteral("𐐷"));
}

QTEST_GUILESS_MAIN(VirtualKeyboardTest)

#include "virtualkeyboardtest.moc"
