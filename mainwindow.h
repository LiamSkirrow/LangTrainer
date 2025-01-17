#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "newdecknameform.h"

// Ui::newdecknameform *addDeckForm;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_deckListWidget_itemClicked(QListWidgetItem *item);
    void on_deckListWidget_addDeckPopup();
    void on_deckListWidget_addDeckPopupClose();
    void on_deckListWidget_addDeckPopupAccept();
    void on_deckListWidget_addDeck(/* string containing name */);

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
