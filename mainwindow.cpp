#include "mainwindow.h"
#include "addlangwindow.h"
#include "./ui_mainwindow.h"
#include "yaml-cpp/yaml.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addLangButton_clicked()
{
    addlangwindow *addlangwin = new addlangwindow(this);
    addlangwin->setModal(true);
    addlangwin->exec();

    YAML::Emitter out;
    out << "Hello, World!";

    qInfo("Here's the output YAML:\n%s", out.c_str());

}

void MainWindow::receive_new_lang(const QString &lang)
{
    ui->langList->setText(lang);
    qInfo("User selected lang");
}
