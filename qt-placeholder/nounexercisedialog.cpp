#include "nounexercisedialog.h"
#include "russiankeyboard.h"

#include <QDialogButtonBox>
#include <QFont>
#include <QFormLayout>
#include <QHash>
#include <QHeaderView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRandomGenerator>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace {

QString normalizedAnswer(const QString &answer)
{
    return answer.simplified().toCaseFolded()
                 .normalized(QString::NormalizationForm_C);
}

} // namespace

NounExerciseDialog::NounExerciseDialog(const NounCatalog &catalog,
                                       const QStringList &selectedNouns,
                                       const QStringList &selectedTargets,
                                       Mode mode, QWidget *parent)
    : QDialog(parent), mode_(mode)
{
    setWindowTitle(mode_ == Mode::Declension
                       ? QStringLiteral("Russian noun declension")
                       : QStringLiteral("Russian prepositions"));
    resize(530, 400);
    setMinimumWidth(460);

    for (const QString &target : selectedTargets) {
        const QString caseKey = mode_ == Mode::Declension
                                    ? target : catalog.prepositionCase.value(target);
        for (const QString &noun : selectedNouns) {
            prompts_.append({noun, catalog.english.value(noun), caseKey,
                             mode_ == Mode::Prepositions
                                 ? catalog.prepositionRussian.value(target) : QString(),
                             mode_ == Mode::Prepositions
                                 ? catalog.prepositionEnglish.value(target) : QString(),
                             catalog.form(noun, caseKey)});
        }
    }
    for (int i = prompts_.size() - 1; i > 0; --i)
        prompts_.swapItemsAt(i, QRandomGenerator::global()->bounded(i + 1));

    auto *layout = new QVBoxLayout(this);
    auto *progressRow = new QHBoxLayout;
    questionNumber_ = new QLabel;
    queueLabel_ = new QLabel;
    progressRow->addWidget(questionNumber_);
    progressRow->addStretch();
    progressRow->addWidget(queueLabel_);
    layout->addLayout(progressRow);

    auto *promptGroup = new QGroupBox(mode_ == Mode::Declension
                                         ? QStringLiteral("Decline the noun")
                                         : QStringLiteral("Decline the noun after the preposition"));
    auto *promptLayout = new QFormLayout(promptGroup);
    nounLabel_ = new QLabel;
    targetLabel_ = new QLabel;
    targetLabel_->setWordWrap(true);
    promptLayout->addRow("Noun:", nounLabel_);
    promptLayout->addRow(mode_ == Mode::Declension ? "Case:" : "Preposition:", targetLabel_);
    promptLayout->addRow("Number:", new QLabel("Singular"));
    layout->addWidget(promptGroup);

    answerEdit_ = new QLineEdit;
    answerEdit_->setPlaceholderText("Type the declined noun only (Cyrillic)");
    layout->addWidget(answerEdit_);
    keyboard_ = createRussianKeyboard(answerEdit_, this);
    layout->addWidget(keyboard_);

    feedbackLabel_ = new QLabel;
    feedbackLabel_->setTextFormat(Qt::PlainText);
    feedbackLabel_->setWordWrap(true);
    feedbackLabel_->setMinimumHeight(feedbackLabel_->fontMetrics().height() * 2);
    layout->addWidget(feedbackLabel_);
    scoreLabel_ = new QLabel;
    layout->addWidget(scoreLabel_);

    auto *buttonRow = new QHBoxLayout;
    checkButton_ = new QPushButton("Check answer");
    showButton_ = new QPushButton("Show answer");
    nextButton_ = new QPushButton("Next question");
    auto *finishButton = new QPushButton("Finish");
    buttonRow->addWidget(checkButton_);
    buttonRow->addWidget(showButton_);
    buttonRow->addWidget(nextButton_);
    buttonRow->addStretch();
    buttonRow->addWidget(finishButton);
    layout->addLayout(buttonRow);

    connect(answerEdit_, &QLineEdit::textChanged, this, [this](const QString &text) {
        checkButton_->setEnabled(answerEdit_->isEnabled() && !text.trimmed().isEmpty());
    });
    connect(answerEdit_, &QLineEdit::returnPressed, this, [this] {
        if (checkButton_->isEnabled()) checkAnswer();
    });
    connect(checkButton_, &QPushButton::clicked, this, [this] { checkAnswer(); });
    connect(showButton_, &QPushButton::clicked, this, [this] { revealAnswer(); });
    connect(nextButton_, &QPushButton::clicked, this, [this] {
        ++questionIndex_;
        if (questionIndex_ == prompts_.size())
            finishSession();
        else
            showQuestion();
    });
    connect(finishButton, &QPushButton::clicked, this, [this] { finishSession(); });
    showQuestion();
}

void NounExerciseDialog::showQuestion()
{
    const Prompt &prompt = prompts_.at(questionIndex_);
    questionNumber_->setText(QStringLiteral("Card %1 of %2")
                                 .arg(questionIndex_ + 1).arg(prompts_.size()));
    queueLabel_->setText(QStringLiteral("Waiting in queue: %1")
                             .arg(prompts_.size() - questionIndex_ - 1));
    nounLabel_->setText(QStringLiteral("%1 — %2").arg(prompt.noun, prompt.meaning));
    targetLabel_->setText(mode_ == Mode::Declension
                              ? russianCaseName(prompt.caseKey)
                              : QStringLiteral("%1 — %2")
                                    .arg(prompt.preposition, prompt.prepositionMeaning));
    answerEdit_->clear();
    answerEdit_->setEnabled(true);
    keyboard_->setEnabled(true);
    feedbackLabel_->clear();
    checkButton_->setEnabled(false);
    showButton_->setEnabled(true);
    nextButton_->setEnabled(false);
    nextButton_->setText(questionIndex_ + 1 == prompts_.size()
                             ? QStringLiteral("View summary") : QStringLiteral("Next question"));
    checkButton_->setDefault(true);
    nextButton_->setDefault(false);
    updateScore();
    answerEdit_->setFocus();
}

void NounExerciseDialog::checkAnswer()
{
    if (answerEdit_->text().trimmed().isEmpty()) return;
    ++attempted_;
    const Prompt &prompt = prompts_.at(questionIndex_);
    const bool correct = normalizedAnswer(answerEdit_->text()) == normalizedAnswer(prompt.answer);
    if (correct)
        ++correct_;
    else
        incorrectCards_.append({prompt, answerEdit_->text().trimmed()});
    feedbackLabel_->setText(QStringLiteral("%1 %2 (%3 case).")
                                .arg(correct ? QStringLiteral("Correct! Form:")
                                             : QStringLiteral("Correct form:"),
                                     prompt.answer, russianCaseName(prompt.caseKey)));
    answerEdit_->setEnabled(false);
    keyboard_->setEnabled(false);
    checkButton_->setEnabled(false);
    showButton_->setEnabled(false);
    nextButton_->setEnabled(true);
    checkButton_->setDefault(false);
    nextButton_->setDefault(true);
    nextButton_->setFocus();
    updateScore();
}

void NounExerciseDialog::revealAnswer()
{
    ++skipped_;
    const Prompt &prompt = prompts_.at(questionIndex_);
    feedbackLabel_->setText(QStringLiteral("Answer: %1 (%2 case).")
                                .arg(prompt.answer, russianCaseName(prompt.caseKey)));
    answerEdit_->setEnabled(false);
    keyboard_->setEnabled(false);
    checkButton_->setEnabled(false);
    showButton_->setEnabled(false);
    nextButton_->setEnabled(true);
    checkButton_->setDefault(false);
    nextButton_->setDefault(true);
    nextButton_->setFocus();
    updateScore();
}

void NounExerciseDialog::updateScore()
{
    scoreLabel_->setText(QStringLiteral("Correct: %1 / %2 tried    Skipped: %3")
                             .arg(correct_).arg(attempted_).arg(skipped_));
}

QString NounExerciseDialog::targetName(const Prompt &prompt) const
{
    if (mode_ == Mode::Prepositions)
        return QStringLiteral("%1 (%2 case)")
            .arg(prompt.preposition, russianCaseName(prompt.caseKey));
    return russianCaseName(prompt.caseKey);
}

void NounExerciseDialog::finishSession()
{
    QDialog summary(this);
    summary.setWindowTitle("Session summary");
    summary.resize(700, 420);
    summary.setMinimumSize(520, 300);

    auto *layout = new QVBoxLayout(&summary);
    auto *heading = new QLabel(mode_ == Mode::Declension
                                   ? QStringLiteral("Russian noun practice — session summary")
                                   : QStringLiteral("Russian preposition practice — session summary"));
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    heading->setFont(headingFont);
    layout->addWidget(heading);

    const int unseen = prompts_.size() - attempted_ - skipped_;
    auto *stats = new QLabel(QStringLiteral(
        "Correct: %1 of %2 attempted    Incorrect: %3    Skipped: %4    Unseen: %5")
        .arg(correct_).arg(attempted_).arg(incorrectCards_.size())
        .arg(skipped_).arg(unseen));
    stats->setWordWrap(true);
    layout->addWidget(stats);
    layout->addWidget(new QLabel(QStringLiteral("Incorrect cards (%1)")
                                     .arg(incorrectCards_.size())));

    if (incorrectCards_.isEmpty()) {
        layout->addWidget(new QLabel("No incorrect answers this session."));
        layout->addStretch();
    } else {
        auto *list = new QTreeWidget;
        list->setHeaderLabels({QStringLiteral("Noun / prompt"),
                               QStringLiteral("Your answer"),
                               QStringLiteral("Correct form")});
        list->setAlternatingRowColors(true);
        list->setUniformRowHeights(true);
        list->setEditTriggers(QAbstractItemView::NoEditTriggers);
        list->setColumnWidth(0, 260);
        list->setColumnWidth(1, 140);
        list->header()->setStretchLastSection(true);

        QHash<QString, QTreeWidgetItem *> groups;
        for (const IncorrectCard &card : incorrectCards_) {
            const QString groupName = targetName(card.prompt);
            auto *group = groups.value(groupName, nullptr);
            if (!group) {
                group = new QTreeWidgetItem(list);
                group->setText(0, groupName);
                QFont groupFont = group->font(0);
                groupFont.setBold(true);
                group->setFont(0, groupFont);
                groups.insert(groupName, group);
            }
            auto *item = new QTreeWidgetItem(group);
            item->setText(0, QStringLiteral("%1 — %2")
                                 .arg(card.prompt.noun, card.prompt.meaning));
            item->setText(1, card.response);
            item->setText(2, card.prompt.answer);
            item->setToolTip(0, item->text(0));
        }
        list->expandAll();
        layout->addWidget(list);
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, &QDialogButtonBox::rejected, &summary, &QDialog::accept);
    layout->addWidget(buttons);
    summary.exec();
    accept();
}
