#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlistwidget.h>
#include "yaml-cpp/yaml.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    std::string str;

public slots:
    void on_addLangButton_clicked();
    void receive_new_lang(const QString &lang);

private slots:
    void on_langList_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    YAML::Node supported_langs;
    YAML::Node lang_database;
};
#endif // MAINWINDOW_H
