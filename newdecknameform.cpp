#include "newdecknameform.h"
#include "ui_newdecknameform.h"

#include <QDebug>

newdecknameform::newdecknameform(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::newdecknameform)
{
    ui->setupUi(this);
}

newdecknameform::~newdecknameform()
{
    delete ui;
}

// close the form with no changes
void newdecknameform::on_buttonBox_rejected()
{
    qDebug() << "form closed";
    // close the form
}

// apply the entered name to the new list item
void newdecknameform::on_buttonBox_accepted()
{
    qDebug() << "form accepted, with text: " << ui->newDeckTextInput->toPlainText();
    // somehow return the name in the form to the mainwindow
}
