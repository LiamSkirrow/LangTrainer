#include "addlangwindow.h"
#include "ui_addlangwindow.h"

addlangwindow::addlangwindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addlangwindow)
{
    ui->setupUi(this);
    // connect the 'ok' button click with the on_buttonBox_accepted() function
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(on_buttonBox_accepted()));
    // connect the emit signal with the parent's receive_new_lang() function
    connect(this, SIGNAL(sendNewLanguageSignal(QString)), parent, SLOT(receive_new_lang(QString)));
}

addlangwindow::~addlangwindow()
{
    delete ui;
}

void addlangwindow::on_buttonBox_accepted()
{
    emit sendNewLanguageSignal("Spanish");
    qInfo("button clicked!");
}
