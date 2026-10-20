#include "verbexercisedialog.h"
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
#include <QToolButton>
#include <QTreeWidget>
#include <QWidget>
#include <QVBoxLayout>

namespace {
QString normalizedAnswer(const QString &answer)
{
    return answer.simplified().toCaseFolded()
                 .normalized(QString::NormalizationForm_C);
}
}

VerbExerciseDialog::VerbExerciseDialog(const VerbCatalog &catalog,
                                       const QStringList &selectedVerbs,
                                       const QStringList &selectedTenses, QWidget *parent)
    : QDialog(parent)
{
    language_ = catalog.language;
    setWindowTitle(QStringLiteral("%1 verb conjugation").arg(language_));
    resize(500, 330);
    setMinimumWidth(410);

    for (const QString &tense : selectedTenses) {
        for (const QString &subject : catalog.subjectsForTense(tense)) {
            const auto forms = catalog.forms.value(
                QStringLiteral("Verbs-%1-%2").arg(tense, subject));
            for (const QString &verb : selectedVerbs) {
                prompts_.append({verb, catalog.english.value(verb), tense, subject,
                                 forms.value(verb)});
            }
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

    auto *promptGroup = new QGroupBox("Conjugate the verb");
    auto *promptLayout = new QFormLayout(promptGroup);
    verbLabel_ = new QLabel;
    tenseLabel_ = new QLabel;
    subjectLabel_ = new QLabel;
    promptLayout->addRow("Infinitive:", verbLabel_);
    promptLayout->addRow("Tense:", tenseLabel_);
    promptLayout->addRow("Subject:", subjectLabel_);
    layout->addWidget(promptGroup);

    answerEdit_ = new QLineEdit;
    answerEdit_->setPlaceholderText(language_ == QLatin1String("Russian")
        ? QStringLiteral("Type the Russian form in Cyrillic")
        : QStringLiteral("Type the Spanish form (include accents)"));
    layout->addWidget(answerEdit_);

    if (language_ == QLatin1String("Russian")) {
        characterRow_ = createRussianKeyboard(answerEdit_, this);
    } else {
        characterRow_ = new QWidget;
        auto *characterLayout = new QHBoxLayout(characterRow_);
        characterLayout->setContentsMargins(0, 0, 0, 0);
        characterLayout->setSpacing(4);
        characterLayout->addWidget(new QLabel("Insert:"));
        for (const QChar character : QStringLiteral("áéíóúüñ")) {
            const QString letter(character);
            auto *button = new QToolButton(characterRow_);
            button->setText(letter);
            button->setToolTip(QStringLiteral("Insert %1").arg(letter));
            button->setFixedWidth(34);
            characterLayout->addWidget(button);
            connect(button, &QToolButton::clicked, this, [this, letter] {
                answerEdit_->insert(letter);
                answerEdit_->setFocus();
            });
        }
        characterLayout->addStretch();
    }
    layout->addWidget(characterRow_);

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
    connect(showButton_, &QPushButton::clicked, this, [this] { revealAnswer(true); });
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

void VerbExerciseDialog::showQuestion()
{
    const Prompt &prompt = prompts_.at(questionIndex_);
    questionNumber_->setText(QStringLiteral("Card %1 of %2")
                                 .arg(questionIndex_ + 1).arg(prompts_.size()));
    queueLabel_->setText(QStringLiteral("Waiting in queue: %1")
                             .arg(prompts_.size() - questionIndex_ - 1));
    verbLabel_->setText(QStringLiteral("%1 — %2").arg(prompt.verb, prompt.meaning));
    tenseLabel_->setText(tenseName(language_, prompt.tense));
    subjectLabel_->setText(subjectName(language_, prompt.subject));
    answerEdit_->clear();
    answerEdit_->setEnabled(true);
    characterRow_->setEnabled(true);
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

void VerbExerciseDialog::checkAnswer()
{
    if (answerEdit_->text().trimmed().isEmpty()) return;
    ++attempted_;
    const QString answer = prompts_.at(questionIndex_).answer;
    const bool correct = normalizedAnswer(answerEdit_->text()) == normalizedAnswer(answer);
    if (correct)
        ++correct_;
    else
        incorrectCards_.append({prompts_.at(questionIndex_), answerEdit_->text().trimmed()});
    feedbackLabel_->setText(correct ? QStringLiteral("Correct! Standard spelling: %1").arg(answer)
                                    : QStringLiteral("Correct answer: %1").arg(answer));
    answerEdit_->setEnabled(false);
    characterRow_->setEnabled(false);
    checkButton_->setEnabled(false);
    showButton_->setEnabled(false);
    nextButton_->setEnabled(true);
    checkButton_->setDefault(false);
    nextButton_->setDefault(true);
    nextButton_->setFocus();
    updateScore();
}

void VerbExerciseDialog::revealAnswer(bool skipped)
{
    if (skipped) ++skipped_;
    feedbackLabel_->setText(QStringLiteral("Answer: %1").arg(prompts_.at(questionIndex_).answer));
    answerEdit_->setEnabled(false);
    characterRow_->setEnabled(false);
    checkButton_->setEnabled(false);
    showButton_->setEnabled(false);
    nextButton_->setEnabled(true);
    checkButton_->setDefault(false);
    nextButton_->setDefault(true);
    nextButton_->setFocus();
    updateScore();
}

void VerbExerciseDialog::updateScore()
{
    scoreLabel_->setText(QStringLiteral("Correct: %1 / %2 tried    Skipped: %3")
                             .arg(correct_).arg(attempted_).arg(skipped_));
}

void VerbExerciseDialog::finishSession()
{
    QDialog summary(this);
    summary.setWindowTitle("Session summary");
    summary.resize(700, 420);
    summary.setMinimumSize(520, 300);

    auto *layout = new QVBoxLayout(&summary);
    auto *heading = new QLabel(QStringLiteral("%1 verb practice — session summary").arg(language_));
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
        list->setHeaderLabels({QStringLiteral("Verb / subject"),
                               QStringLiteral("Your answer"),
                               QStringLiteral("Correct form")});
        list->setAlternatingRowColors(true);
        list->setUniformRowHeights(true);
        list->setEditTriggers(QAbstractItemView::NoEditTriggers);
        list->setColumnWidth(0, 260);
        list->setColumnWidth(1, 140);
        list->header()->setStretchLastSection(true);

        QHash<QString, QTreeWidgetItem *> tenseGroups;
        for (const IncorrectCard &card : incorrectCards_) {
            auto *group = tenseGroups.value(card.prompt.tense, nullptr);
            if (!group) {
                group = new QTreeWidgetItem(list);
                group->setText(0, tenseName(language_, card.prompt.tense));
                QFont groupFont = group->font(0);
                groupFont.setBold(true);
                group->setFont(0, groupFont);
                tenseGroups.insert(card.prompt.tense, group);
            }
            auto *item = new QTreeWidgetItem(group);
            item->setText(0, QStringLiteral("%1 (%2) — %3")
                                 .arg(card.prompt.verb, card.prompt.meaning,
                                      subjectName(language_, card.prompt.subject)));
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
