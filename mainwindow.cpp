#include "mainwindow.h"
#include "addlangwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // read in the language spec YAML and populate the list of available languages
    YAML::Node lang_spec = YAML::LoadFile("../../spec.yaml");
    YAML::Node lang_db = YAML::LoadFile("../../language.yaml");
    supported_langs = lang_spec["specs"];
    lang_database = lang_db["languages"];

    // iterate over the supported langs and add to the list
    switch (supported_langs.Type()) {
    case YAML::NodeType::Null: qInfo("supported_langs is a Null type"); break;
    case YAML::NodeType::Scalar: qInfo("supported_langs is a Scalar type"); break;
    case YAML::NodeType::Sequence: qInfo("supported_langs is a Sequence type"); break;
    case YAML::NodeType::Map: qInfo("supported_langs is a Map type"); break;
    case YAML::NodeType::Undefined: qInfo("supported_langs is a Undefined type"); break;
    }
    qInfo("size: %zu", supported_langs.size());

    switch (lang_database.Type()) {
    case YAML::NodeType::Null: qInfo("lang_database is a Null type"); break;
    case YAML::NodeType::Scalar: qInfo("lang_database is a Scalar type"); break;
    case YAML::NodeType::Sequence: qInfo("lang_database is a Sequence type"); break;
    case YAML::NodeType::Map: qInfo("lang_database is a Map type"); break;
    case YAML::NodeType::Undefined: qInfo("lang_database is a Undefined type"); break;
    }
    qInfo("size: %zu", lang_database.size());

    // With for-range loop
    for (auto child : supported_langs) {
        qInfo( "found language in spec.YAML: %s", child.first.as<std::string>().c_str() );
        ui->langList->addItem(child.first.as<std::string>().c_str());
    }

    // ui->langList->

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

    // YAML::Node lang_spec = YAML::LoadFile("../../spec.yaml");

    // if (lang_spec["specs"]["Spanish"]) {
    //     qInfo("Spanish detected!");
    // }
    // else{
    //     qInfo("Spanish not detected!");
    // }

    // const YAML::Node num_genders = lang_spec["specs"]["Spanish"]["numGenders"];
    // switch (num_genders.Type()) {
    // case YAML::NodeType::Null: qInfo("num_genders is a Null type"); break;
    // case YAML::NodeType::Scalar: qInfo("num_genders is a Scalar type"); break;
    // case YAML::NodeType::Sequence: qInfo("num_genders is a Sequence type"); break;
    // case YAML::NodeType::Map: qInfo("num_genders is a Map type"); break;
    // case YAML::NodeType::Undefined: qInfo("num_genders is a Undefined type"); break;
    // }
    // qInfo("numGenders: %d", num_genders.as<int>());

    // const YAML::Node gender_list = lang_spec["specs"]["Spanish"]["genders"];
    // switch (gender_list.Type()) {
    // case YAML::NodeType::Null: qInfo("gender_list is a Null type"); break;
    // case YAML::NodeType::Scalar: qInfo("gender_list is a Scalar type"); break;
    // case YAML::NodeType::Sequence: qInfo("gender_list is a Sequence type"); break;
    // case YAML::NodeType::Map: qInfo("gender_list is a Map type"); break;
    // case YAML::NodeType::Undefined: qInfo("gender_list is a Undefined type"); break;
    // }
    // std::list<std::string> gender_vec = gender_list.as<std::list<std::string>>();

    // for(std::string str : gender_vec){
    //     qInfo("numGenders: %s", str.c_str());
    // }

    // qInfo("genders: %d", gender_list.as<int>());

    // std::ofstream fout("config.yaml");
    // fout << config;

}

void MainWindow::receive_new_lang(const QString &lang)
{

}

void MainWindow::on_langList_itemClicked(QListWidgetItem *item)
{
    qInfo("User selected lang: %s", item->text().toStdString().c_str());
}

