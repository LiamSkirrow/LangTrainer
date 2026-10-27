#pragma once

#include "pronoundata.h"

#include <QDialog>
#include <QVector>

class QLabel;
class QLineEdit;
class QPushButton;
class QWidget;

class PronounExerciseDialog : public QDialog {
public:
    PronounExerciseDialog(const PronounCatalog &catalog,
                          const QStringList &selectedPronouns,
                          const QStringList &selectedCases,
                          QWidget *parent = nullptr);

private:
    struct Prompt {
        QString pronoun;
        QString displayName;
        QString kind;
        QString caseKey;
        QString use;
        QString referent;
        QStringList answers;
    };
    struct IncorrectCard {
        Prompt prompt;
        QString response;
    };

    void showQuestion();
    void checkAnswer();
    void revealAnswer();
    void updateScore();
    void finishSession();
    QString contextName(const Prompt &prompt) const;

    QVector<Prompt> prompts_;
    QVector<IncorrectCard> incorrectCards_;
    int questionIndex_ = 0;
    int attempted_ = 0;
    int correct_ = 0;
    int skipped_ = 0;
    QLabel *questionNumber_ = nullptr;
    QLabel *queueLabel_ = nullptr;
    QLabel *pronounLabel_ = nullptr;
    QLabel *caseLabel_ = nullptr;
    QLabel *contextLabel_ = nullptr;
    QLabel *feedbackLabel_ = nullptr;
    QLabel *scoreLabel_ = nullptr;
    QLineEdit *answerEdit_ = nullptr;
    QWidget *keyboard_ = nullptr;
    QPushButton *checkButton_ = nullptr;
    QPushButton *showButton_ = nullptr;
    QPushButton *nextButton_ = nullptr;
};
