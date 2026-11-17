#pragma once

#include "adjectivedata.h"

#include <QDialog>

class AdjectiveFormsDialog : public QDialog {
public:
    AdjectiveFormsDialog(const AdjectiveCatalog &catalog, const QString &adjective,
                         QWidget *parent = nullptr);
};
