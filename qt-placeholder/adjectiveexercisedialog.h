#pragma once

#include "adjectivedata.h"
#include "adjectivetargetsdialog.h"

#include <QDialog>
#include <QVector>

class QLabel;
class QLineEdit;
class QPushButton;
class QWidget;

class AdjectiveExerciseDialog : public QDialog {
public:
    AdjectiveExerciseDialog(const AdjectiveCatalog &catalog,
                            const QStringList &selectedAdjectives,
                            const AdjectiveTargets &targets, QWidget *parent = nullptr);

private:
    struct Prompt {
        QString adjective;
        QString meaning;
        QString pattern;
        QString caseKey;
        QString referent;
        QString phrase;
        QString answer;
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
    QLabel *adjectiveLabel_ = nullptr;
    QLabel *contextLabel_ = nullptr;
    QLabel *phraseLabel_ = nullptr;
    QLabel *feedbackLabel_ = nullptr;
    QLabel *scoreLabel_ = nullptr;
    QLineEdit *answerEdit_ = nullptr;
    QWidget *keyboard_ = nullptr;
    QPushButton *checkButton_ = nullptr;
    QPushButton *showButton_ = nullptr;
    QPushButton *nextButton_ = nullptr;
};
