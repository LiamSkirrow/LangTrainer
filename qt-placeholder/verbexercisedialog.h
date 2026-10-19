#pragma once

#include "verbdata.h"

#include <QDialog>
#include <QVector>

class QLabel;
class QLineEdit;
class QPushButton;

class VerbExerciseDialog : public QDialog {
public:
    VerbExerciseDialog(const VerbCatalog &catalog, const QStringList &selectedVerbs,
                       QWidget *parent = nullptr);

private:
    struct Prompt {
        QString verb;
        QString meaning;
        QString tense;
        QString subject;
        QString answer;
    };

    void showQuestion();
    void checkAnswer();
    void revealAnswer(bool skipped);
    void updateScore();

    QVector<Prompt> prompts_;
    int questionIndex_ = 0;
    int attempted_ = 0;
    int correct_ = 0;
    int skipped_ = 0;
    QLabel *questionNumber_ = nullptr;
    QLabel *verbLabel_ = nullptr;
    QLabel *tenseLabel_ = nullptr;
    QLabel *subjectLabel_ = nullptr;
    QLabel *feedbackLabel_ = nullptr;
    QLabel *scoreLabel_ = nullptr;
    QLineEdit *answerEdit_ = nullptr;
    QPushButton *checkButton_ = nullptr;
    QPushButton *showButton_ = nullptr;
    QPushButton *nextButton_ = nullptr;
};
