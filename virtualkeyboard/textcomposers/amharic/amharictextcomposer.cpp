/*
    SPDX-FileCopyrightText: 2026 Abenezer Wesenseged <wseged@proton.me>
    SPDX-FileCopyrightText: 2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "amharictextcomposer.h"

#include "inputengine.h"

AmharicTextComposer::AmharicTextComposer(QObject *parent)
    : DirectTextComposer(parent)
{
}

bool AmharicTextComposer::keyEvent(Qt::Key key, const QString &text)
{
    auto *engine = inputEngine();
    if (!engine) {
        return false;
    }

    if (!preeditText().isEmpty()) {
        if (key == Qt::Key_Backspace || key == Qt::Key_Escape) {
            engine->clear();
            return true;
        }
        engine->commit();
    }

    static const QStringList syllableFamilies = {
        QStringLiteral("ሀሁሂሃሄህሆሇ"), QStringLiteral("ለሉሊላሌልሎሏ"), QStringLiteral("ሐሑሒሓሔሕሖሗ"), QStringLiteral("መሙሚማሜምሞሟ"), QStringLiteral("ሠሡሢሣሤሥሦሧ"),
        QStringLiteral("ረሩሪራሬርሮሯ"), QStringLiteral("ሰሱሲሳሴስሶሷ"), QStringLiteral("ሸሹሺሻሼሽሾሿ"), QStringLiteral("ቀቁቂቃቄቅቆቇ"), QStringLiteral("በቡቢባቤብቦቧ"),
        QStringLiteral("ቨቩቪቫቬቭቮቯ"), QStringLiteral("ተቱቲታቴትቶቷ"), QStringLiteral("ቸቹቺቻቼችቾቿ"), QStringLiteral("ኀኁኂኃኄኅኆኇ"), QStringLiteral("ነኑኒናኔንኖኗ"),
        QStringLiteral("ኘኙኚኛኜኝኞኟ"), QStringLiteral("አኡኢኣኤእኦኧ"), QStringLiteral("ከኩኪካኬክኮኯ"), QStringLiteral("ኰኲኳኴኵ"),    QStringLiteral("ኸኹኺኻኼኽኾዃ"),
        QStringLiteral("ወዉዊዋዌውዎዏ"), QStringLiteral("ዐዑዒዓዔዕዖ"),  QStringLiteral("ዘዙዚዛዜዝዞዟ"), QStringLiteral("ዠዡዢዣዤዥዦዧ"), QStringLiteral("የዩዪያዬይዮዯ"),
        QStringLiteral("ደዱዲዳዴድዶዷ"), QStringLiteral("ጀጁጂጃጄጅጆጇ"), QStringLiteral("ገጉጊጋጌግጎጏ"), QStringLiteral("ጠጡጢጣጤጥጦጧ"), QStringLiteral("ጨጩጪጫጬጭጮጯ"),
        QStringLiteral("ጰጱጲጳጴጵጶጷ"), QStringLiteral("ጸጹጺጻጼጽጾጿ"), QStringLiteral("ፀፁፂፃፄፅፆፇ"), QStringLiteral("ፈፉፊፋፌፍፎፏ"), QStringLiteral("ፐፑፒፓፔፕፖፗ"),
        QStringLiteral("ቈቊቋቌቍ"),    QStringLiteral("ኈኊኋኌኍ"),    QStringLiteral("ዀዂዃዄዅ"),    QStringLiteral("ጐጒጓጔጕ"),
    };
    if (text.size() == 1) {
        for (const auto &family : syllableFamilies) {
            if (family.front() != text.front()) {
                continue;
            }
            QStringList forms;
            forms.reserve(family.size());
            for (const auto character : family) {
                forms.append(QString(character));
            }
            setPreeditText(text);
            setCandidates(forms);
            return true;
        }
    }

    return DirectTextComposer::keyEvent(key, text);
}

bool AmharicTextComposer::selectCandidate(int index)
{
    auto forms = candidates();
    if (!inputEngine() || index < 0 || index >= forms.size()) {
        return false;
    }
    inputEngine()->commit(forms.at(index));
    return true;
}
