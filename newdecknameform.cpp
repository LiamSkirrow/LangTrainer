#include "newdecknameform.h"
#include "ui_newdecknameform.h"

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
