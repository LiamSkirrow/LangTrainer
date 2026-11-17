#pragma once

#include <QDialog>
#include <QHash>
#include <QStringList>

class QCheckBox;
class QComboBox;
class QLabel;
class QPushButton;
class QVBoxLayout;

struct AdjectiveTargets {
    QStringList patterns;
    QStringList cases;
    QStringList referents;
    int sessionLimit = 30; // Zero means the entire selected deck.
};

class AdjectiveTargetsDialog : public QDialog {
public:
    explicit AdjectiveTargetsDialog(int adjectiveCount, QWidget *parent = nullptr);
    AdjectiveTargets targets() const;

private:
    void addChecks(QVBoxLayout *layout, const QString &setting,
                   const QStringList &keys, const QStringList &labels,
                   QHash<QString, QCheckBox *> &destination);
    QStringList checked(const QStringList &keys,
                        const QHash<QString, QCheckBox *> &boxes) const;
    void updateCount();

    int adjectiveCount_ = 0;
    QHash<QString, QCheckBox *> patternBoxes_;
    QHash<QString, QCheckBox *> caseBoxes_;
    QHash<QString, QCheckBox *> referentBoxes_;
    QComboBox *sessionLimit_ = nullptr;
    QLabel *countLabel_ = nullptr;
    QPushButton *startButton_ = nullptr;
};
