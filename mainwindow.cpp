#include "newdecknameform.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // TODO: figure out why this gives a warning with No matching signal for on_deckListWidget_addDeck
    // connect(ui->pushButton_addDeck, SIGNAL(clicked()), this, SLOT(on_deckListWidget_addDeck()));
    connect(ui->pushButton_addDeck, SIGNAL(clicked()), this, SLOT(on_deckListWidget_addDeckPopup()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "howdy ho 1";
}

void MainWindow::on_pushButton_2_clicked()
{
    qDebug() << "howdy ho 2";
}

void MainWindow::on_pushButton_3_clicked()
{
    qDebug() << "howdy ho 3";
}

// on click: highlight/select the option
void MainWindow::on_deckListWidget_itemClicked(QListWidgetItem *item)
{
    qDebug() << "item selected with text: " << item->text();
}

void MainWindow::on_deckListWidget_addDeckPopup()
{
    newdecknameform *addDeckDialog = new newdecknameform;
    addDeckDialog->exec();
    QString deckName = addDeckDialog->getDeckText();
    // TODO: need to check if the cancel button was pressed
    if(!addDeckDialog->Rejected){
        ui->deckListWidget->addItem(deckName);
    }

    delete(addDeckDialog);

    // debug
    qDebug() << "New deck created, with name: " << deckName;
}








