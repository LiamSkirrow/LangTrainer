#pragma once

#include "verbdata.h"

#include <QDialog>

class VerbFormsDialog : public QDialog {
public:
    VerbFormsDialog(const VerbCatalog &catalog, const QString &verb,
                    QWidget *parent = nullptr);
};
