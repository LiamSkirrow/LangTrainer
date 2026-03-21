#include "mainwindow.h"
#include "addlangwindow.h"
#include "./ui_mainwindow.h"
#include <QApplication>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // read in the language spec YAML and populate the list of available languages
    // MacOS/Linux
    // YAML::Node lang_spec = YAML::LoadFile("../../../../../spec.yaml");
    // YAML::Node lang_db = YAML::LoadFile("../../../../../language.yaml");
    // Windows
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

}

void MainWindow::receive_new_lang(const QString &lang)
{

}

void MainWindow::on_langList_itemClicked(QListWidgetItem *item)
{
    std::string selected_lang_name = item->text().toStdString();
    qInfo("User selected lang: %s", selected_lang_name.c_str());

    // lookup the selected language and grab the vocab from the lang_database
    YAML::Node selected_lang_handle = lang_database[selected_lang_name];

    // add vocab relevant to selected language to the ui elem langDetailTree
    // using model based approach rather than item based

    auto *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels({"Vocab", "Details"});

    auto *verbs_item = new QStandardItem("Verbs");
    auto *nouns_item = new QStandardItem("Nouns");
    auto *adjs_item  = new QStandardItem("Adjectives");
    auto *preps_item = new QStandardItem("Prepositions");

    for(auto verb : selected_lang_handle["Verbs"]){
        verbs_item->appendRow(new QStandardItem(verb.as<std::string>().c_str()));
        qInfo("User selected lang has verb: %s", verb.as<std::string>().c_str());
    }

    // TODO: refactor language.yaml to store nouns: {madre: f, padre: m}
    for(auto noun : selected_lang_handle["Nouns"]){
        nouns_item->appendRow(new QStandardItem(noun.as<std::string>().c_str()));
        qInfo("User selected lang has noun: %s", noun.as<std::string>().c_str());
    }

    for(auto adj : selected_lang_handle["Adjectives"]){
        adjs_item->appendRow(new QStandardItem(adj.as<std::string>().c_str()));
        qInfo("User selected lang has adj: %s", adj.as<std::string>().c_str());
    }

    for(auto prep : selected_lang_handle["Prepositions"]){
        preps_item->appendRow(new QStandardItem(prep.as<std::string>().c_str()));
        qInfo("User selected lang has prep: %s", prep.as<std::string>().c_str());
    }

    model->appendRow(verbs_item);
    model->appendRow(nouns_item);
    model->appendRow(adjs_item);
    model->appendRow(preps_item);

    // QTreeView view;
    ui->langDetailTree->setModel(model);
    ui->langDetailTree->expandAll();

    QItemSelectionModel *langDetailTreeSelModel = ui->langDetailTree->selectionModel();
    connect(langDetailTreeSelModel,
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this,
            SLOT(testSlot(const QModelIndex &, const QModelIndex &)));

    // TODO: tree
    // - give collapse all button and retract all button
}

void MainWindow::testSlot(const QModelIndex &curr, const QModelIndex &prev){
    auto parent = curr.parent();
    bool parentPresent = true;
    if(parent == QModelIndex()){
        parentPresent = false;
    }
    qInfo("TEST SLOT TRIGGERED: %d %d %d", parentPresent, curr.row(), curr.column());
    qInfo("   Selected word: " + ui->langDetailTree->model()->data(curr).toString().toLatin1());
    if(parentPresent){
        qInfo("   belongs to word class: " + ui->langDetailTree->model()->data(curr.parent()).toString().toLatin1());
    }
}
