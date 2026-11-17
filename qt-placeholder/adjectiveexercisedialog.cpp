#include "adjectiveexercisedialog.h"
#include "germankeyboard.h"

#include <QDialogButtonBox>
#include <QFont>
#include <QGroupBox>
#include <QHash>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSizePolicy>
#include <QTreeWidget>
#include <QVBoxLayout>

namespace {

QString normalizedAnswer(const QString &answer)
{
    return answer.simplified().normalized(QString::NormalizationForm_C);
}

void addField(QVBoxLayout *layout, const QString &caption, QLabel *value)
{
    auto *heading = new QLabel(caption);
    QFont font = heading->font();
    font.setBold(true);
    heading->setFont(font);
    layout->addWidget(heading);
    value->setTextFormat(Qt::PlainText);
    value->setWordWrap(true);
    value->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(value);
    layout->addSpacing(7);
}

} // namespace

AdjectiveExerciseDialog::AdjectiveExerciseDialog(const AdjectiveCatalog &catalog,
                                                 const QStringList &selectedAdjectives,
                                                 const AdjectiveTargets &targets,
                                                 QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("German adjective endings"));
    resize(660, 500);
    setMinimumSize(500, 450);

    QHash<QString, QStringList> nounChoices;
    nounChoices.insert(QStringLiteral("pl"), catalog.nouns.nouns);
    for (const QString &noun : catalog.nouns.nouns)
        nounChoices[catalog.nouns.gender.value(noun)].append(noun);

    for (int a = 0; a < selectedAdjectives.size(); ++a) {
        const QString &adjective = selectedAdjectives.at(a);
        for (int p = 0; p < targets.patterns.size(); ++p) {
            const QString &pattern = targets.patterns.at(p);
            for (int c = 0; c < targets.cases.size(); ++c) {
                const QString &caseKey = targets.cases.at(c);
                for (int r = 0; r < targets.referents.size(); ++r) {
                    const QString &referent = targets.referents.at(r);
                    const QStringList choices = nounChoices.value(referent);
                    const QString noun = choices.at((a + p + c + r) % choices.size());
                    const QString number = referent == QLatin1String("pl")
                                               ? QStringLiteral("plural") : QStringLiteral("singular");
                    const QString declined = catalog.nouns.form(noun, caseKey, number);
                    const QString nounForm = declined.mid(declined.indexOf(QLatin1Char(' ')) + 1);
                    const QString determiner = adjectiveDeterminer(
                        pattern, caseKey, referent, a + p + c + r);
                    const QString phrase = determiner.isEmpty()
                        ? QStringLiteral("___ %1").arg(nounForm)
                        : QStringLiteral("%1 ___ %2").arg(determiner, nounForm);
                    prompts_.append({adjective, catalog.english.value(adjective), pattern,
                                     caseKey, referent, phrase,
                                     catalog.form(adjective, pattern, caseKey, referent)});
                }
            }
        }
    }
    for (int i = prompts_.size() - 1; i > 0; --i)
        prompts_.swapItemsAt(i, QRandomGenerator::global()->bounded(i + 1));
    if (targets.sessionLimit > 0 && prompts_.size() > targets.sessionLimit)
        prompts_.resize(targets.sessionLimit);

    auto *layout = new QVBoxLayout(this);
    auto *progressRow = new QHBoxLayout;
    questionNumber_ = new QLabel;
    queueLabel_ = new QLabel;
    progressRow->addWidget(questionNumber_);
    progressRow->addStretch();
    progressRow->addWidget(queueLabel_);
    layout->addLayout(progressRow);

    auto *promptGroup = new QGroupBox("Fill in the adjective");
    auto *promptLayout = new QVBoxLayout(promptGroup);
    promptLayout->setContentsMargins(14, 16, 14, 14);
    promptLayout->setSpacing(3);
    adjectiveLabel_ = new QLabel;
    contextLabel_ = new QLabel;
    phraseLabel_ = new QLabel;
    addField(promptLayout, QStringLiteral("Base adjective"), adjectiveLabel_);
    addField(promptLayout, QStringLiteral("Case and noun type"), contextLabel_);
    addField(promptLayout, QStringLiteral("Noun phrase"), phraseLabel_);
    QFont phraseFont = phraseLabel_->font();
    phraseFont.setPointSize(phraseFont.pointSize() + 2);
    phraseLabel_->setFont(phraseFont);
    layout->addWidget(promptGroup);

    answerEdit_ = new QLineEdit;
    answerEdit_->setPlaceholderText("Type the declined adjective (include umlauts and ß)");
    layout->addWidget(answerEdit_);
    keyboard_ = createGermanKeyboard(answerEdit_, this);
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
        if (questionIndex_ == prompts_.size()) finishSession();
        else showQuestion();
    });
    connect(finishButton, &QPushButton::clicked, this, [this] { finishSession(); });
    showQuestion();
}

QString AdjectiveExerciseDialog::contextName(const Prompt &prompt) const
{
    return QStringLiteral("%1 · %2")
        .arg(caseName(prompt.caseKey), adjectiveReferentName(prompt.referent));
}

void AdjectiveExerciseDialog::showQuestion()
{
    const Prompt &prompt = prompts_.at(questionIndex_);
    questionNumber_->setText(QStringLiteral("Card %1 of %2")
                                 .arg(questionIndex_ + 1).arg(prompts_.size()));
    queueLabel_->setText(QStringLiteral("Waiting in queue: %1")
                             .arg(prompts_.size() - questionIndex_ - 1));
    adjectiveLabel_->setText(QStringLiteral("%1 — %2")
                                 .arg(prompt.adjective, prompt.meaning));
    contextLabel_->setText(contextName(prompt));
    phraseLabel_->setText(prompt.phrase);
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

void AdjectiveExerciseDialog::checkAnswer()
{
    if (answerEdit_->text().trimmed().isEmpty()) return;
    ++attempted_;
    const Prompt &prompt = prompts_.at(questionIndex_);
    const bool correct = normalizedAnswer(answerEdit_->text()) == normalizedAnswer(prompt.answer);
    if (correct) ++correct_;
    else incorrectCards_.append({prompt, answerEdit_->text().trimmed()});
    feedbackLabel_->setText(QStringLiteral("%1 %2. %3.")
                                .arg(correct ? QStringLiteral("Correct! Form:")
                                             : QStringLiteral("Correct form:"),
                                     prompt.answer, adjectivePatternName(prompt.pattern)));
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

void AdjectiveExerciseDialog::revealAnswer()
{
    ++skipped_;
    const Prompt &prompt = prompts_.at(questionIndex_);
    feedbackLabel_->setText(QStringLiteral("Answer: %1. %2.")
                                .arg(prompt.answer, adjectivePatternName(prompt.pattern)));
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

void AdjectiveExerciseDialog::updateScore()
{
    scoreLabel_->setText(QStringLiteral("Correct: %1 / %2 tried    Skipped: %3")
                             .arg(correct_).arg(attempted_).arg(skipped_));
}

void AdjectiveExerciseDialog::finishSession()
{
    QDialog summary(this);
    summary.setWindowTitle("Session summary");
    summary.resize(820, 440);
    summary.setMinimumSize(560, 300);
    auto *layout = new QVBoxLayout(&summary);
    auto *heading = new QLabel("German adjective endings — session summary");
    QFont font = heading->font();
    font.setBold(true);
    heading->setFont(font);
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
        list->setHeaderLabels({QStringLiteral("Prompt"), QStringLiteral("Your answer"),
                               QStringLiteral("Correct form")});
        list->setAlternatingRowColors(true);
        list->setUniformRowHeights(true);
        list->setEditTriggers(QAbstractItemView::NoEditTriggers);
        list->setColumnWidth(0, 480);
        list->setColumnWidth(1, 130);
        list->header()->setStretchLastSection(true);
        QHash<QString, QTreeWidgetItem *> groups;
        for (const IncorrectCard &card : incorrectCards_) {
            auto *group = groups.value(card.prompt.pattern, nullptr);
            if (!group) {
                group = new QTreeWidgetItem(list);
                group->setText(0, adjectivePatternName(card.prompt.pattern));
                QFont groupFont = group->font(0);
                groupFont.setBold(true);
                group->setFont(0, groupFont);
                groups.insert(card.prompt.pattern, group);
            }
            auto *item = new QTreeWidgetItem(group);
            item->setText(0, QStringLiteral("%1: %2 (%3)")
                                 .arg(card.prompt.adjective, card.prompt.phrase,
                                      contextName(card.prompt)));
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
