#include "mainwindow.h"
#include "addlangwindow.h"
#include "./ui_mainwindow.h"
#include <QApplication>
#include <QItemSelectionModel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>
// #include <string>

namespace {

void resetLanguageView(Ui::MainWindow *ui)
{
    if (QAbstractItemModel *existingModel = ui->langDetailTree->model()) {
        ui->langDetailTree->setModel(nullptr);
        delete existingModel;
    }

    ui->vocabInfoOutput->clear();
}

bool appendWordClass(QStandardItemModel *model,
                     const QString &label,
                     const YAML::Node &words)
{
    if (!words || !words.IsSequence() || words.size() == 0) {
        return false;
    }

    auto *categoryItem = new QStandardItem(label);
    for (const auto &word : words) {
        categoryItem->appendRow(new QStandardItem(word.as<std::string>().c_str()));
    }

    model->appendRow(categoryItem);
    return true;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // read in the language spec YAML and populate the list of available languages
    // MacOS/Linux
    YAML::Node lang_spec = YAML::LoadFile("../../../../../spec.yaml");
    YAML::Node lang_db = YAML::LoadFile("../../../../../language.yaml");
    // Windows
    // YAML::Node lang_spec = YAML::LoadFile("../../spec.yaml");
    // YAML::Node lang_db = YAML::LoadFile("../../language.yaml");

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

    ui->vocabInfoOutput->setReadOnly(true);

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

    resetLanguageView(ui);
    selected_lang_handle = YAML::Node();

    // lookup the selected language and grab the vocab from the lang_database
    const YAML::Node languageDatabase = lang_database;
    const YAML::Node selectedLanguage = languageDatabase[selected_lang_name];
    if (!selectedLanguage || !selectedLanguage.IsMap()) {
        ui->vocabInfoOutput->setText(
            QString("No vocabulary is loaded for %1 yet.").arg(item->text()));
        return;
    }

    selected_lang_handle = selectedLanguage;

    // add vocab relevant to selected language to the ui elem langDetailTree
    // using model based approach rather than item based

    auto *model = new QStandardItemModel(ui->langDetailTree);
    model->setHorizontalHeaderLabels({"Vocab"});

    bool hasVocabulary = false;

    if (appendWordClass(model, "Verbs", selected_lang_handle["Verbs"])) {
        hasVocabulary = true;
    }

    if (appendWordClass(model, "Nouns", selected_lang_handle["Nouns"])) {
        hasVocabulary = true;
    }

    if (appendWordClass(model, "Adjectives", selected_lang_handle["Adjectives"])) {
        hasVocabulary = true;
    }

    if (appendWordClass(model, "Prepositions", selected_lang_handle["Prepositions"])) {
        hasVocabulary = true;
    }

    if (!hasVocabulary) {
        delete model;
        selected_lang_handle = YAML::Node();
        ui->vocabInfoOutput->setText(
            QString("No vocabulary entries are available for %1 yet.").arg(item->text()));
        return;
    }

    // QTreeView view;
    ui->langDetailTree->setModel(model);
    ui->langDetailTree->expandAll();

    if (QItemSelectionModel *langDetailTreeSelModel = ui->langDetailTree->selectionModel()) {
        connect(langDetailTreeSelModel,
                &QItemSelectionModel::currentChanged,
                this,
                &MainWindow::testSlot);
    }

    // TODO: tree
    // - give collapse all button and retract all button
}

void MainWindow::testSlot(const QModelIndex &curr, const QModelIndex &prev){
    auto parent = curr.parent();
    // const char *vocab =
    bool parentPresent = true;
    YAML::Node vocab_sample;
    std::stringstream vocab_str;

    if(parent == QModelIndex()){
        parentPresent = false;
    }
    qInfo("TEST SLOT TRIGGERED: %d %d %d", parentPresent, curr.row(), curr.column());
    qInfo("   Selected word: " + ui->langDetailTree->model()->data(curr).toString().toLatin1());
    if(parentPresent){
        qInfo("   belongs to word class: " + ui->langDetailTree->model()->data(curr.parent()).toString().toLatin1());
    }

    // we have to perform the YAML Map lookup with std::string as index
    vocab_str << ui->langDetailTree->model()->data(curr).toString().toLatin1().data();
    std::string vocab_std_str = vocab_str.str();

    // clear the output display
    ui->vocabInfoOutput->setText("");

    //  determine what kind of vocab is currently selected
    if(ui->langDetailTree->model()->data(curr.parent()).toString().toLatin1() == "Verbs"){
        // fetch the conjugated versions of the selected vocab
        // ... create a std::list of 'Verbs-en-inf' etc and iterate over it, looking up the required vocab each time
        ui->vocabInfoOutput->append("Verb selected");
    }
    else if(ui->langDetailTree->model()->data(curr.parent()).toString().toLatin1() == "Nouns"){
        // fetch the noun genders
        vocab_sample = selected_lang_handle["Nouns-gender"][vocab_std_str];
        ui->vocabInfoOutput->append(vocab_sample.as<std::string>().c_str());
    }
    else if(ui->langDetailTree->model()->data(curr.parent()).toString().toLatin1() == "Adjectives"){
        ui->vocabInfoOutput->append("No extra info to display");
    }
    else if(ui->langDetailTree->model()->data(curr.parent()).toString().toLatin1() == "Prepositions"){
        ui->vocabInfoOutput->append("No extra info to display");
    }
    else{
        // do nothing, a parent must've been selected -> no action
    }

}
