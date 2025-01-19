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
    QString getDeckText();

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
private:
    Ui::newdecknameform *ui;
};

#endif // NEWDECKNAMEFORM_H
