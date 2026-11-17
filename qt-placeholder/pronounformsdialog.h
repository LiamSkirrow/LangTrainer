#pragma once

#include "pronoundata.h"

#include <QDialog>

class PronounFormsDialog : public QDialog {
public:
    PronounFormsDialog(const PronounCatalog &catalog, const QString &pronoun,
                       QWidget *parent = nullptr);
};
