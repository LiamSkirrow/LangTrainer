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

    YAML::Node lang_spec = YAML::LoadFile("../../spec.yaml");

    if (lang_spec["specs"]["Spanish"]) {
        qInfo("Spanish detected!");
    }
    else{
        qInfo("Spanish not detected!");
    }

    const YAML::Node num_genders = lang_spec["specs"]["Spanish"]["numGenders"];
    switch (num_genders.Type()) {
    case YAML::NodeType::Null: qInfo("num_genders is a Null type"); break;
    case YAML::NodeType::Scalar: qInfo("num_genders is a Scalar type"); break;
    case YAML::NodeType::Sequence: qInfo("num_genders is a Sequence type"); break;
    case YAML::NodeType::Map: qInfo("num_genders is a Map type"); break;
    case YAML::NodeType::Undefined: qInfo("num_genders is a Undefined type"); break;
    }
    qInfo("numGenders: %d", num_genders.as<int>());

    const YAML::Node gender_list = lang_spec["specs"]["Spanish"]["genders"];
    switch (gender_list.Type()) {
    case YAML::NodeType::Null: qInfo("gender_list is a Null type"); break;
    case YAML::NodeType::Scalar: qInfo("gender_list is a Scalar type"); break;
    case YAML::NodeType::Sequence: qInfo("gender_list is a Sequence type"); break;
    case YAML::NodeType::Map: qInfo("gender_list is a Map type"); break;
    case YAML::NodeType::Undefined: qInfo("gender_list is a Undefined type"); break;
    }
    std::list<std::string> gender_vec = gender_list.as<std::list<std::string>>();

    for(std::string str : gender_vec){
        qInfo("numGenders: %s", str.c_str());
    }

    // qInfo("genders: %d", gender_list.as<int>());

    // std::ofstream fout("config.yaml");
    // fout << config;

}

void MainWindow::receive_new_lang(const QString &lang)
{
    ui->langList->setText(lang);
    qInfo("User selected lang");
}
