#pragma once

#include "noundata.h"

#include <QDialog>
#include <QVector>

class QLabel;
class QLineEdit;
class QPushButton;
class QWidget;

class NounExerciseDialog : public QDialog {
public:
    enum class Mode { Declension, Prepositions };

    NounExerciseDialog(const NounCatalog &catalog, const QStringList &selectedNouns,
                       const QStringList &selectedTargets, Mode mode, QWidget *parent = nullptr);

private:
    struct Prompt {
        QString noun;
        QString meaning;
        QString caseKey;
        QString preposition;
        QString prepositionMeaning;
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
    QString targetName(const Prompt &prompt) const;

    Mode mode_;
    QVector<Prompt> prompts_;
    QVector<IncorrectCard> incorrectCards_;
    int questionIndex_ = 0;
    int attempted_ = 0;
    int correct_ = 0;
    int skipped_ = 0;
    QLabel *questionNumber_ = nullptr;
    QLabel *queueLabel_ = nullptr;
    QLabel *nounLabel_ = nullptr;
    QLabel *targetLabel_ = nullptr;
    QLabel *feedbackLabel_ = nullptr;
    QLabel *scoreLabel_ = nullptr;
    QLineEdit *answerEdit_ = nullptr;
    QWidget *keyboard_ = nullptr;
    QPushButton *checkButton_ = nullptr;
    QPushButton *showButton_ = nullptr;
    QPushButton *nextButton_ = nullptr;
};
