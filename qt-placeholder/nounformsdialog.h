#pragma once

#include "noundata.h"

#include <QDialog>

class NounFormsDialog : public QDialog {
public:
    NounFormsDialog(const NounCatalog &catalog, const QString &noun,
                    QWidget *parent = nullptr);
};
