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

// !!! TODO: might be able to delete this !!!
// slot to handle creation of new deck when 'Add Deck' button is pressed
void MainWindow::on_deckListWidget_addDeck()
{
    qDebug() << "new deck button clicked";

    // trigger popup window to enter in name for new deck
    qDebug() << "new deck added";
    ui->deckListWidget->addItem("Test deck!");

    // need to check for name collisions

}

void MainWindow::on_deckListWidget_addDeckPopup()
{
    qDebug() << "dialogue popup";
    // addDeckForm->exec();
    // QDialog *addDeckDialog = new QDialog;
    // addDeckDialog->exec();
    newdecknameform *addDeckDialog = new newdecknameform;
    addDeckDialog->exec();
}

