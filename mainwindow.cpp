#include "mainwindow.h"
#include "addlangwindow.h"
#include "./ui_mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>
// #include <string>

namespace {

QString resolveProjectFilePath(const QString &fileName)
{
    const QDir appDir(QCoreApplication::applicationDirPath());
    const QStringList candidates = {
        appDir.filePath("../../../../../" + fileName),
        appDir.filePath("../../" + fileName),
        QDir::current().filePath(fileName)
    };

    for (const QString &candidate : candidates) {
        if (QFileInfo::exists(candidate)) {
            return QFileInfo(candidate).absoluteFilePath();
        }
    }

    return {};
}

void resetLanguageView(Ui::MainWindow *ui)
{
    if (QAbstractItemModel *existingModel = ui->langDetailTree->model()) {
        ui->langDetailTree->setModel(nullptr);
        delete existingModel;
    }

    ui->vocabInfoOutput->clear();
}

YAML::Node loadLanguageData(const QString &databasePath, const std::string &languageName)
{
    if (databasePath.isEmpty()) {
        return {};
    }

    const YAML::Node databaseRoot = YAML::LoadFile(databasePath.toStdString());
    const YAML::Node languages = databaseRoot["languages"];

    if (!languages || !languages.IsMap()) {
        return {};
    }

    return languages[languageName];
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
    const QString specPath = resolveProjectFilePath("spec.yaml");
    language_db_path = resolveProjectFilePath("language.yaml");
    const YAML::Node lang_spec = YAML::LoadFile(specPath.toStdString());
    supported_langs = lang_spec["specs"];

    // iterate over the supported langs and add to the list
    switch (supported_langs.Type()) {
    case YAML::NodeType::Null: qInfo("supported_langs is a Null type"); break;
    case YAML::NodeType::Scalar: qInfo("supported_langs is a Scalar type"); break;
    case YAML::NodeType::Sequence: qInfo("supported_langs is a Sequence type"); break;
    case YAML::NodeType::Map: qInfo("supported_langs is a Map type"); break;
    case YAML::NodeType::Undefined: qInfo("supported_langs is a Undefined type"); break;
    }
    qInfo("size: %zu", supported_langs.size());

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
    current_language_name = selected_lang_name;

    // lookup the selected language and grab the vocab from the language database file
    const YAML::Node selectedLanguage = loadLanguageData(language_db_path, selected_lang_name);
    if (!selectedLanguage || !selectedLanguage.IsMap()) {
        ui->vocabInfoOutput->setText(
            QString("No vocabulary is loaded for %1 yet.").arg(item->text()));
        return;
    }

    // add vocab relevant to selected language to the ui elem langDetailTree
    // using model based approach rather than item based

    auto *model = new QStandardItemModel(ui->langDetailTree);
    model->setHorizontalHeaderLabels({"Vocab"});

    bool hasVocabulary = false;

    if (appendWordClass(model, "Verbs", selectedLanguage["Verbs"])) {
        hasVocabulary = true;
    }

    if (appendWordClass(model, "Nouns", selectedLanguage["Nouns"])) {
        hasVocabulary = true;
    }

    if (appendWordClass(model, "Adjectives", selectedLanguage["Adjectives"])) {
        hasVocabulary = true;
    }

    if (appendWordClass(model, "Prepositions", selectedLanguage["Prepositions"])) {
        hasVocabulary = true;
    }

    if (!hasVocabulary) {
        delete model;
        ui->vocabInfoOutput->setText(
            QString("No vocabulary entries are available for %1 yet.").arg(item->text()));
        return;
    }

    // QTreeView view;
    ui->langDetailTree->setModel(model);
    ui->langDetailTree->expandAll();

    // TODO: tree
    // - give collapse all button and retract all button
}

void MainWindow::on_langDetailTree_clicked(const QModelIndex &curr)
{
    if (!curr.isValid() || !ui->langDetailTree->model()) {
        return;
    }

    auto parent = curr.parent();
    if (!parent.isValid()) {
        ui->vocabInfoOutput->setText("");
        return;
    }

    const YAML::Node selectedLanguage = loadLanguageData(language_db_path, current_language_name);
    if (!selectedLanguage || !selectedLanguage.IsMap()) {
        ui->vocabInfoOutput->setText("No vocabulary is loaded for the selected language.");
        return;
    }

    YAML::Node vocab_sample;
    std::stringstream vocab_str;

    qInfo("TEST SLOT TRIGGERED: %d %d", curr.row(), curr.column());
    qInfo("%s", ("   Selected word: " + ui->langDetailTree->model()->data(curr).toString()).toUtf8().constData());
    qInfo("%s", ("   belongs to word class: "
                 + ui->langDetailTree->model()->data(curr.parent()).toString()).toUtf8().constData());

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
        vocab_sample = selectedLanguage["Nouns-gender"][vocab_std_str];
        if (vocab_sample && vocab_sample.IsScalar()) {
            ui->vocabInfoOutput->append(vocab_sample.as<std::string>().c_str());
        } else {
            ui->vocabInfoOutput->append("No noun details available");
        }
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
