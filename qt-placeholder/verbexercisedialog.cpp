#include "verbexercisedialog.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QVBoxLayout>

namespace {
QString displayTense(const QString &tense)
{
    if (tense == QLatin1String("present")) return QStringLiteral("Present");
    if (tense == QLatin1String("present_cont")) return QStringLiteral("Present progressive");
    if (tense == QLatin1String("simple_past")) return QStringLiteral("Preterite");
    if (tense == QLatin1String("imperfect_past")) return QStringLiteral("Imperfect");
    if (tense == QLatin1String("present_perf")) return QStringLiteral("Present perfect");
    if (tense == QLatin1String("pluperfect")) return QStringLiteral("Past perfect");
    if (tense == QLatin1String("future")) return QStringLiteral("Future");
    if (tense == QLatin1String("future_perf")) return QStringLiteral("Future perfect");
    if (tense == QLatin1String("conditional")) return QStringLiteral("Conditional");
    if (tense == QLatin1String("conditional_perf")) return QStringLiteral("Conditional perfect");
    QString label = tense;
    return label.replace(QLatin1Char('_'), QLatin1Char(' '));
}

QString displaySubject(const QString &subject)
{
    if (subject == QLatin1String("1psing")) return QStringLiteral("yo");
    if (subject == QLatin1String("1ppl")) return QStringLiteral("nosotros / nosotras");
    if (subject == QLatin1String("2pfam")) return QStringLiteral("tú");
    if (subject == QLatin1String("2ppol")) return QStringLiteral("usted");
    if (subject == QLatin1String("2ppl")) return QStringLiteral("ustedes");
    if (subject == QLatin1String("3psing")) return QStringLiteral("él / ella");
    if (subject == QLatin1String("3ppl")) return QStringLiteral("ellos / ellas");
    return subject;
}

QString normalizedAnswer(const QString &answer)
{
    return answer.simplified().normalized(QString::NormalizationForm_C).toCaseFolded();
}
}

VerbExerciseDialog::VerbExerciseDialog(const VerbCatalog &catalog,
                                       const QStringList &selectedVerbs, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Spanish verb conjugation");
    resize(500, 330);
    setMinimumWidth(410);

    for (const QString &tense : catalog.tenses) {
        for (const QString &subject : catalog.subjects) {
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
    questionNumber_ = new QLabel;
    layout->addWidget(questionNumber_);

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
    answerEdit_->setPlaceholderText("Type the Spanish form");
    layout->addWidget(answerEdit_);

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
        if (questionIndex_ == prompts_.size()) {
            QMessageBox::information(this, "Practice complete",
                QStringLiteral("You answered %1 of %2 attempts correctly and skipped %3 questions.")
                    .arg(correct_).arg(attempted_).arg(skipped_));
            accept();
        } else {
            showQuestion();
        }
    });
    connect(finishButton, &QPushButton::clicked, this, &QDialog::accept);
    showQuestion();
}

void VerbExerciseDialog::showQuestion()
{
    const Prompt &prompt = prompts_.at(questionIndex_);
    questionNumber_->setText(QStringLiteral("Question %1").arg(questionIndex_ + 1));
    verbLabel_->setText(QStringLiteral("%1 — %2").arg(prompt.verb, prompt.meaning));
    tenseLabel_->setText(displayTense(prompt.tense));
    subjectLabel_->setText(displaySubject(prompt.subject));
    answerEdit_->clear();
    answerEdit_->setEnabled(true);
    feedbackLabel_->clear();
    checkButton_->setEnabled(false);
    showButton_->setEnabled(true);
    nextButton_->setEnabled(false);
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
    if (correct) ++correct_;
    feedbackLabel_->setText(correct ? QStringLiteral("Correct!")
                                    : QStringLiteral("Correct answer: %1").arg(answer));
    answerEdit_->setEnabled(false);
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
