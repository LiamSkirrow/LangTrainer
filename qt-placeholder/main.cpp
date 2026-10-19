#include "verbdata.h"
#include "verbexercisedialog.h"
#include "selectiondialog.h"

#include <QApplication>
#include <QDebug>
#include <QComboBox>
#include <QFont>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QMainWindow>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace {

struct PracticeMode {
    const char *name;
    const char *description;
    const char *buttonText;
};

class MainWindow : public QMainWindow {
public:
    MainWindow()
    {
        setWindowTitle("LangTrainer");
        resize(780, 560);
        setMinimumSize(650, 500);

        auto *central = new QWidget(this);
        auto *layout = new QVBoxLayout(central);
        layout->setContentsMargins(20, 18, 20, 20);
        layout->setSpacing(14);

        auto *title = new QLabel("LangTrainer");
        QFont titleFont = title->font();
        titleFont.setPointSize(titleFont.pointSize() + 5);
        titleFont.setBold(true);
        title->setFont(titleFont);
        layout->addWidget(title);
        layout->addWidget(new QLabel("Choose a language, then choose what to practise."));

        auto *languageGroup = new QGroupBox("Language");
        auto *languageLayout = new QHBoxLayout(languageGroup);
        languageLayout->addWidget(new QLabel("Language:"));

        auto *languagePicker = new QComboBox;
        languagePicker->addItem("Select a language...");
        languagePicker->addItem("Spanish (Español)");
        languagePicker->setMinimumWidth(220);

        // A dedicated popup view and palette keep the choices readable on dark themes.
        languagePicker->setView(new QListView(languagePicker));
        QPalette pickerPalette = languagePicker->palette();
        pickerPalette.setColor(QPalette::Base, Qt::white);
        pickerPalette.setColor(QPalette::Button, Qt::white);
        pickerPalette.setColor(QPalette::Text, Qt::black);
        pickerPalette.setColor(QPalette::ButtonText, Qt::black);
        pickerPalette.setColor(QPalette::Highlight, QColor(45, 103, 173));
        pickerPalette.setColor(QPalette::HighlightedText, Qt::white);
        languagePicker->setPalette(pickerPalette);
        languagePicker->view()->setPalette(pickerPalette);

        languageLayout->addWidget(languagePicker);
        languageLayout->addStretch();
        layout->addWidget(languageGroup);

        auto *selectionHint = new QLabel("Select a language to see its practice modes.");
        layout->addWidget(selectionHint);

        auto *practiceGroup = new QGroupBox("Spanish practice modes");
        auto *practiceLayout = new QGridLayout(practiceGroup);
        practiceLayout->setHorizontalSpacing(18);
        practiceLayout->setVerticalSpacing(13);
        practiceLayout->setColumnStretch(1, 1);

        const PracticeMode modes[] = {
            {"Verb conjugation", "Practise verb forms across people and tenses.", "Choose verbs..."},
            {"Vocabulary", "Review useful words and their meanings.", "Practise"},
            {"Sentence practice", "Use words and grammar together in context.", "Practise"},
            {"Listening and speaking", "Respond to spoken prompts.", "Practise"},
            {"Noun declension", "For languages with noun cases; not used in Spanish.", "Not available"},
        };

        int row = 0;
        for (const auto &mode : modes) {
            auto *name = new QLabel(QString::fromUtf8(mode.name));
            QFont nameFont = name->font();
            nameFont.setBold(true);
            name->setFont(nameFont);
            practiceLayout->addWidget(name, row, 0);

            auto *description = new QLabel(QString::fromUtf8(mode.description));
            description->setWordWrap(true);
            practiceLayout->addWidget(description, row, 1);

            auto *button = new QPushButton(QString::fromUtf8(mode.buttonText));
            button->setMinimumWidth(110);
            button->setEnabled(row == 0);
            if (row == 0) {
                connect(button, &QPushButton::clicked, this, [this] { startVerbPractice(); });
            }
            practiceLayout->addWidget(button, row, 2);
            ++row;
        }

        practiceGroup->hide();
        layout->addWidget(practiceGroup);
        layout->addStretch();
        setCentralWidget(central);

        connect(languagePicker, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [practiceGroup, selectionHint](int index) {
                    const bool spanishSelected = index == 1;
                    practiceGroup->setVisible(spanishSelected);
                    selectionHint->setText(spanishSelected
                        ? "Spanish is selected. Choose verbs and tenses to start conjugation practice."
                        : "Select a language to see its practice modes.");
                });
    }

private:
    void startVerbPractice()
    {
        VerbCatalog catalog;
        QString error;
        if (!loadSpanishCatalog(catalog, error)) {
            QMessageBox::critical(this, "Vocabulary error", error);
            return;
        }

        SelectionDialogConfig verbConfig;
        verbConfig.title = "Choose Spanish verbs";
        verbConfig.instruction = "Choose the verbs for this practice session.";
        verbConfig.searchPlaceholder = "Search verbs or English meanings...";
        verbConfig.itemName = "verbs";
        verbConfig.settingsKey = "Spanish/verbConjugation/selectedVerbs";
        verbConfig.continueLabel = "Next: tenses";
        QStringList sortedVerbs = catalog.verbs;
        sortedVerbs.sort(Qt::CaseInsensitive);
        for (const QString &verb : sortedVerbs) {
            verbConfig.options.append(
                {verb, QStringLiteral("%1 — %2").arg(verb, catalog.english.value(verb))});
        }
        SelectionDialog verbs(verbConfig, this);
        if (verbs.exec() != QDialog::Accepted)
            return;

        SelectionDialogConfig tenseConfig;
        tenseConfig.title = "Choose Spanish tenses";
        tenseConfig.instruction = "Choose the tenses for this practice session.";
        tenseConfig.searchPlaceholder = "Search tenses...";
        tenseConfig.itemName = "tenses";
        tenseConfig.settingsKey = "Spanish/verbConjugation/selectedTenses";
        tenseConfig.continueLabel = "Start practice";
        for (const QString &tense : catalog.tenses)
            tenseConfig.options.append({tense, spanishTenseName(tense)});
        SelectionDialog tenses(tenseConfig, this);
        if (tenses.exec() != QDialog::Accepted)
            return;

        VerbExerciseDialog exercise(catalog, verbs.selectedValues(), tenses.selectedValues(), this);
        exercise.exec();
    }
};

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("LangTrainer");
    QApplication::setApplicationName("LangTrainer");

    if (app.arguments().contains(QStringLiteral("--check-data"))) {
        VerbCatalog catalog;
        QString error;
        if (!loadSpanishCatalog(catalog, error)) {
            qCritical().noquote() << error;
            return 1;
        }
        qInfo().noquote() << QStringLiteral("Loaded %1 Spanish verbs, %2 tenses, and %3 subjects.")
                                 .arg(catalog.verbs.size()).arg(catalog.tenses.size())
                                 .arg(catalog.subjects.size());
        return 0;
    }

    MainWindow window;
    window.show();
    return app.exec();
}
