#ifndef NEWDECKNAMEFORM_H
#define NEWDECKNAMEFORM_H

#include <QDialog>

namespace Ui {
class newdecknameform;
}

class newdecknameform : public QDialog
{
    Q_OBJECT

public:
    explicit newdecknameform(QWidget *parent = nullptr);
    ~newdecknameform();

private:
    Ui::newdecknameform *ui;
};

#endif // NEWDECKNAMEFORM_H
