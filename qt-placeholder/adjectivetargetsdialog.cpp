#include "adjectivetargetsdialog.h"
#include "adjectivedata.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSet>
#include <QSettings>
#include <QVBoxLayout>

namespace {
const QString settingsPrefix = QStringLiteral("German/adjectiveEndings/");
}

AdjectiveTargetsDialog::AdjectiveTargetsDialog(int adjectiveCount, QWidget *parent)
    : QDialog(parent), adjectiveCount_(adjectiveCount)
{
    setWindowTitle(QStringLiteral("Choose German adjective targets"));
    resize(540, 480);
    setMinimumSize(440, 420);
    auto *layout = new QVBoxLayout(this);
    auto *instruction = new QLabel(
        "Choose which article patterns, cases, and noun types to practise. "
        "The card shows the article and noun; type only the adjective.");
    instruction->setWordWrap(true);
    layout->addWidget(instruction);

    auto *patterns = new QGroupBox("Article pattern");
    auto *patternLayout = new QVBoxLayout(patterns);
    QStringList patternLabels;
    for (const QString &key : adjectivePatterns()) patternLabels.append(adjectivePatternName(key));
    addChecks(patternLayout, settingsPrefix + QStringLiteral("patterns"),
              adjectivePatterns(), patternLabels, patternBoxes_);
    layout->addWidget(patterns);

    auto *filters = new QHBoxLayout;
    auto *cases = new QGroupBox("Case");
    auto *caseLayout = new QVBoxLayout(cases);
    const QStringList caseKeys = {QStringLiteral("nominative"), QStringLiteral("accusative"),
                                  QStringLiteral("dative"), QStringLiteral("genitive")};
    QStringList caseLabels;
    for (const QString &key : caseKeys) caseLabels.append(caseName(key));
    addChecks(caseLayout, settingsPrefix + QStringLiteral("cases"),
              caseKeys, caseLabels, caseBoxes_);
    filters->addWidget(cases);

    auto *referents = new QGroupBox("Noun type");
    auto *referentLayout = new QVBoxLayout(referents);
    QStringList referentLabels;
    for (const QString &key : adjectiveReferents())
        referentLabels.append(adjectiveReferentName(key));
    addChecks(referentLayout, settingsPrefix + QStringLiteral("referents"),
              adjectiveReferents(), referentLabels, referentBoxes_);
    filters->addWidget(referents);
    layout->addLayout(filters);

    auto *sessionRow = new QHBoxLayout;
    sessionRow->addWidget(new QLabel("Cards this session:"));
    sessionLimit_ = new QComboBox;
    sessionLimit_->addItem("30 cards", 30);
    sessionLimit_->addItem("60 cards", 60);
    sessionLimit_->addItem("Entire selected deck", 0);
    QSettings settings;
    const int savedLimit = settings.value(settingsPrefix + QStringLiteral("sessionLimit"), 30).toInt();
    const int savedIndex = sessionLimit_->findData(savedLimit);
    sessionLimit_->setCurrentIndex(savedIndex >= 0 ? savedIndex : 0);
    sessionRow->addWidget(sessionLimit_);
    sessionRow->addStretch();
    layout->addLayout(sessionRow);
    countLabel_ = new QLabel;
    countLabel_->setWordWrap(true);
    layout->addWidget(countLabel_);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel);
    startButton_ = buttons->addButton("Start practice", QDialogButtonBox::AcceptRole);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(startButton_, &QPushButton::clicked, this, [this] {
        const AdjectiveTargets chosen = targets();
        QSettings settings;
        settings.setValue(settingsPrefix + QStringLiteral("patterns"), chosen.patterns);
        settings.setValue(settingsPrefix + QStringLiteral("cases"), chosen.cases);
        settings.setValue(settingsPrefix + QStringLiteral("referents"), chosen.referents);
        settings.setValue(settingsPrefix + QStringLiteral("sessionLimit"), chosen.sessionLimit);
        accept();
    });
    connect(sessionLimit_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { updateCount(); });
    layout->addWidget(buttons);
    updateCount();
}

void AdjectiveTargetsDialog::addChecks(QVBoxLayout *layout, const QString &setting,
                                       const QStringList &keys, const QStringList &labels,
                                       QHash<QString, QCheckBox *> &destination)
{
    QSettings settings;
    const bool hasSaved = settings.contains(setting);
    QSet<QString> saved;
    for (const QString &value : settings.value(setting).toStringList()) saved.insert(value);
    for (int i = 0; i < keys.size(); ++i) {
        auto *box = new QCheckBox(labels.at(i));
        box->setChecked(!hasSaved || saved.contains(keys.at(i)));
        destination.insert(keys.at(i), box);
        layout->addWidget(box);
        connect(box, &QCheckBox::toggled, this, [this] { updateCount(); });
    }
}

QStringList AdjectiveTargetsDialog::checked(
    const QStringList &keys, const QHash<QString, QCheckBox *> &boxes) const
{
    QStringList result;
    for (const QString &key : keys)
        if (boxes.value(key)->isChecked()) result.append(key);
    return result;
}

AdjectiveTargets AdjectiveTargetsDialog::targets() const
{
    AdjectiveTargets result;
    result.patterns = checked(adjectivePatterns(), patternBoxes_);
    result.cases = checked({QStringLiteral("nominative"), QStringLiteral("accusative"),
                            QStringLiteral("dative"), QStringLiteral("genitive")}, caseBoxes_);
    result.referents = checked(adjectiveReferents(), referentBoxes_);
    result.sessionLimit = sessionLimit_->currentData().toInt();
    return result;
}

void AdjectiveTargetsDialog::updateCount()
{
    const AdjectiveTargets chosen = targets();
    const int possible = adjectiveCount_ * chosen.patterns.size() * chosen.cases.size() *
                         chosen.referents.size();
    const int cards = chosen.sessionLimit == 0 ? possible : qMin(possible, chosen.sessionLimit);
    countLabel_->setText(QStringLiteral("%1 possible cards; %2 cards in this session.")
                             .arg(possible).arg(cards));
    startButton_->setEnabled(possible > 0);
}
