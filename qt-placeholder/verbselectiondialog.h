#pragma once

#include "verbdata.h"

#include <QDialog>
#include <QStringList>

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;

class VerbSelectionDialog : public QDialog {
public:
    explicit VerbSelectionDialog(const VerbCatalog &catalog, QWidget *parent = nullptr);
    QStringList selectedVerbs() const;

private:
    void updateCount();

    QListWidget *verbList_ = nullptr;
    QLabel *countLabel_ = nullptr;
    QPushButton *startButton_ = nullptr;
};
