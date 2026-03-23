#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QModelIndex>
#include <QMainWindow>
#include <qlistwidget.h>
#include <QString>
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
    void on_langDetailTree_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    YAML::Node supported_langs;
    QString language_db_path;
    std::string current_language_name;
};
#endif // MAINWINDOW_H
