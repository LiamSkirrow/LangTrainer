#include "pronounexercisedialog.h"
#include "germankeyboard.h"
#include "noundata.h"

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
#include <QWidget>

namespace {

QString normalizedAnswer(const QString &answer)
{
    return answer.simplified().normalized(QString::NormalizationForm_C);
}

QString displayAnswers(const QStringList &answers)
{
    return answers.join(QStringLiteral(" / "));
}

void addField(QVBoxLayout *layout, const QString &caption, QLabel *value)
{
    auto *heading = new QLabel(caption);
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    heading->setFont(headingFont);
    layout->addWidget(heading);
    value->setTextFormat(Qt::PlainText);
    value->setWordWrap(true);
    value->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(value);
    layout->addSpacing(7);
}

} // namespace

PronounExerciseDialog::PronounExerciseDialog(const PronounCatalog &catalog,
                                             const QStringList &selectedPronouns,
                                             const QStringList &selectedCases,
                                             QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("German pronoun declension"));
    resize(620, 480);
    setMinimumSize(480, 440);

    for (const QString &caseKey : selectedCases) {
        for (const QString &pronoun : selectedPronouns) {
            const QString kind = catalog.kind.value(pronoun);
            if (kind == QLatin1String("personal")) {
                prompts_.append({pronoun, catalog.displayName(pronoun), kind, caseKey,
                                 QString(), QString(), catalog.answers(pronoun, caseKey)});
            } else {
                for (const QString &use : {QStringLiteral("before"), QStringLiteral("alone")}) {
                    for (const QString &referent : {QStringLiteral("m"), QStringLiteral("f"),
                                                    QStringLiteral("n"), QStringLiteral("pl")}) {
                        prompts_.append({pronoun, catalog.displayName(pronoun), kind, caseKey,
                                         use, referent,
                                         catalog.answers(pronoun, caseKey, use, referent)});
                    }
                }
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

    auto *promptGroup = new QGroupBox("Decline the pronoun");
    auto *promptLayout = new QVBoxLayout(promptGroup);
    promptLayout->setContentsMargins(14, 16, 14, 14);
    promptLayout->setSpacing(3);
    pronounLabel_ = new QLabel;
    caseLabel_ = new QLabel;
    contextLabel_ = new QLabel;
    addField(promptLayout, QStringLiteral("Pronoun / owner"), pronounLabel_);
    addField(promptLayout, QStringLiteral("Case"), caseLabel_);
    addField(promptLayout, QStringLiteral("Use and referent"), contextLabel_);
    layout->addWidget(promptGroup);

    answerEdit_ = new QLineEdit;
    answerEdit_->setPlaceholderText("Type the pronoun form (include capitals)");
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
        if (questionIndex_ == prompts_.size())
            finishSession();
        else
            showQuestion();
    });
    connect(finishButton, &QPushButton::clicked, this, [this] { finishSession(); });
    showQuestion();
}

QString PronounExerciseDialog::contextName(const Prompt &prompt) const
{
    if (prompt.kind == QLatin1String("personal"))
        return QStringLiteral("Personal pronoun");
    return QStringLiteral("%1; %2")
        .arg(pronounUseName(prompt.use), pronounReferentName(prompt.referent));
}

void PronounExerciseDialog::showQuestion()
{
    const Prompt &prompt = prompts_.at(questionIndex_);
    questionNumber_->setText(QStringLiteral("Card %1 of %2")
                                 .arg(questionIndex_ + 1).arg(prompts_.size()));
    queueLabel_->setText(QStringLiteral("Waiting in queue: %1")
                             .arg(prompts_.size() - questionIndex_ - 1));
    pronounLabel_->setText(prompt.displayName);
    caseLabel_->setText(prompt.caseKey == QLatin1String("genitive") &&
                                prompt.kind == QLatin1String("personal")
                            ? QStringLiteral("Genitive (rare in everyday speech)")
                            : caseName(prompt.caseKey));
    contextLabel_->setText(contextName(prompt));
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

void PronounExerciseDialog::checkAnswer()
{
    if (answerEdit_->text().trimmed().isEmpty()) return;
    ++attempted_;
    const Prompt &prompt = prompts_.at(questionIndex_);
    bool correct = false;
    for (const QString &form : prompt.answers) {
        if (normalizedAnswer(answerEdit_->text()) == normalizedAnswer(form)) {
            correct = true;
            break;
        }
    }
    if (correct)
        ++correct_;
    else
        incorrectCards_.append({prompt, answerEdit_->text().trimmed()});
    feedbackLabel_->setText(QStringLiteral("%1 %2 (%3 case).")
                                .arg(correct ? QStringLiteral("Correct! Form:")
                                             : QStringLiteral("Correct form:"),
                                     displayAnswers(prompt.answers), caseName(prompt.caseKey)));
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

void PronounExerciseDialog::revealAnswer()
{
    ++skipped_;
    const Prompt &prompt = prompts_.at(questionIndex_);
    feedbackLabel_->setText(QStringLiteral("Answer: %1 (%2 case).")
                                .arg(displayAnswers(prompt.answers), caseName(prompt.caseKey)));
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

void PronounExerciseDialog::updateScore()
{
    scoreLabel_->setText(QStringLiteral("Correct: %1 / %2 tried    Skipped: %3")
                             .arg(correct_).arg(attempted_).arg(skipped_));
}

void PronounExerciseDialog::finishSession()
{
    QDialog summary(this);
    summary.setWindowTitle("Session summary");
    summary.resize(720, 440);
    summary.setMinimumSize(520, 300);
    auto *layout = new QVBoxLayout(&summary);
    auto *heading = new QLabel("German pronoun practice — session summary");
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
        list->setHeaderLabels({QStringLiteral("Pronoun / context"),
                               QStringLiteral("Your answer"),
                               QStringLiteral("Correct form")});
        list->setAlternatingRowColors(true);
        list->setUniformRowHeights(true);
        list->setEditTriggers(QAbstractItemView::NoEditTriggers);
        list->setColumnWidth(0, 350);
        list->setColumnWidth(1, 130);
        list->header()->setStretchLastSection(true);
        QHash<QString, QTreeWidgetItem *> groups;
        for (const IncorrectCard &card : incorrectCards_) {
            const QString groupName = caseName(card.prompt.caseKey);
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
                                 .arg(card.prompt.displayName, contextName(card.prompt)));
            item->setText(1, card.response);
            item->setText(2, displayAnswers(card.prompt.answers));
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
