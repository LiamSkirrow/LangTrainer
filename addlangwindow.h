#ifndef ADDLANGWINDOW_H
#define ADDLANGWINDOW_H

#include <QDialog>

namespace Ui {
class addlangwindow;
}

class addlangwindow : public QDialog
{
    Q_OBJECT

public:
    explicit addlangwindow(QWidget *parent = nullptr);
    ~addlangwindow();

public: signals:
    void sendNewLanguageSignal(const QString &lang);

public slots:
    void on_buttonBox_accepted();

private:
    Ui::addlangwindow *ui;
};

#endif // ADDLANGWINDOW_H
