#include "verbdata.h"
#include "verbexercisedialog.h"
#include "verbformsdialog.h"
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

QPushButton *addPracticeRow(QGridLayout *layout, int row, const QString &name,
                            const QString &description, const QString &buttonText,
                            bool available)
{
    auto *nameLabel = new QLabel(name);
    QFont nameFont = nameLabel->font();
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);
    layout->addWidget(nameLabel, row, 0);

    auto *descriptionLabel = new QLabel(description);
    descriptionLabel->setWordWrap(true);
    layout->addWidget(descriptionLabel, row, 1);

    auto *button = new QPushButton(buttonText);
    button->setMinimumWidth(110);
    button->setEnabled(available);
    layout->addWidget(button, row, 2);
    return button;
}

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
        languagePicker->addItem("Spanish (Español)", QStringLiteral("Spanish"));
        languagePicker->addItem("Russian (Русский)", QStringLiteral("Russian"));
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

        auto *spanishGroup = createSpanishPracticeGroup();
        auto *russianGroup = createRussianPracticeGroup();
        spanishGroup->hide();
        russianGroup->hide();
        layout->addWidget(spanishGroup);
        layout->addWidget(russianGroup);
        layout->addStretch();
        setCentralWidget(central);

        connect(languagePicker, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [languagePicker, spanishGroup, russianGroup, selectionHint](int) {
                    const QString language = languagePicker->currentData().toString();
                    spanishGroup->setVisible(language == QLatin1String("Spanish"));
                    russianGroup->setVisible(language == QLatin1String("Russian"));
                    selectionHint->setText(language.isEmpty()
                        ? "Select a language to see its practice modes."
                        : QStringLiteral("%1 is selected. Choose verbs and tenses to start conjugation practice.")
                              .arg(language));
                });
    }

private:
    QGroupBox *createSpanishPracticeGroup()
    {
        auto *group = new QGroupBox("Spanish practice modes");
        auto *grid = new QGridLayout(group);
        grid->setHorizontalSpacing(18);
        grid->setVerticalSpacing(13);
        grid->setColumnStretch(1, 1);

        auto *verbButton = addPracticeRow(grid, 0, "Verb conjugation",
            "Practise verb forms across people and tenses.", "Choose verbs...", true);
        connect(verbButton, &QPushButton::clicked, this, [this] { startVerbPractice(QStringLiteral("Spanish")); });
        addPracticeRow(grid, 1, "Sentence practice",
            "Use words and grammar together in context.", "Practise", false);
        addPracticeRow(grid, 2, "Listening and speaking",
            "Respond to spoken prompts.", "Practise", false);
        return group;
    }

    QGroupBox *createRussianPracticeGroup()
    {
        auto *group = new QGroupBox("Russian practice modes");
        auto *grid = new QGridLayout(group);
        grid->setHorizontalSpacing(18);
        grid->setVerticalSpacing(13);
        grid->setColumnStretch(1, 1);

        auto *verbButton = addPracticeRow(grid, 0, "Verb conjugation",
            "Practise verb forms across people and tenses.", "Choose verbs...", true);
        connect(verbButton, &QPushButton::clicked, this, [this] { startVerbPractice(QStringLiteral("Russian")); });
        addPracticeRow(grid, 1, "Noun declension",
            "Practise noun forms across Russian cases.", "Practise", false);
        addPracticeRow(grid, 2, "Sentence practice",
            "Use words and grammar together in context.", "Practise", false);
        addPracticeRow(grid, 3, "Listening and speaking",
            "Respond to spoken prompts.", "Practise", false);
        return group;
    }

    void startVerbPractice(const QString &language)
    {
        VerbCatalog catalog;
        QString error;
        if (!loadVerbCatalog(language, catalog, error)) {
            QMessageBox::critical(this, "Vocabulary error", error);
            return;
        }

        SelectionDialogConfig verbConfig;
        verbConfig.title = QStringLiteral("Choose %1 verbs").arg(language);
        verbConfig.instruction = "Check verbs to practise. Highlight one to view its forms.";
        verbConfig.searchPlaceholder = "Search verbs or English meanings...";
        verbConfig.itemName = "verbs";
        verbConfig.settingsKey = language + QStringLiteral("/verbConjugation/selectedVerbs");
        verbConfig.continueLabel = "Next: tenses";
        verbConfig.detailsLabel = "View forms...";
        verbConfig.showDetails = [&catalog](const QString &verb, QWidget *parent) {
            VerbFormsDialog forms(catalog, verb, parent);
            forms.exec();
        };
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
        tenseConfig.title = QStringLiteral("Choose %1 tenses").arg(language);
        tenseConfig.instruction = "Choose the tenses for this practice session.";
        tenseConfig.searchPlaceholder = "Search tenses...";
        tenseConfig.itemName = "tenses";
        tenseConfig.settingsKey = language + QStringLiteral("/verbConjugation/selectedTenses");
        tenseConfig.continueLabel = "Start practice";
        for (const QString &tense : catalog.tenses)
            tenseConfig.options.append({tense, tenseName(language, tense)});
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
        for (const QString &language : {QStringLiteral("Spanish"), QStringLiteral("Russian")}) {
            VerbCatalog catalog;
            QString error;
            if (!loadVerbCatalog(language, catalog, error)) {
                qCritical().noquote() << error;
                return 1;
            }
            int cardsPerVerb = 0;
            for (const QString &tense : catalog.tenses)
                cardsPerVerb += catalog.subjectsForTense(tense).size();
            qInfo().noquote() << QStringLiteral("Loaded %1 %2 verbs, %3 tenses, and %4 cards per verb.")
                                     .arg(catalog.verbs.size()).arg(language)
                                     .arg(catalog.tenses.size()).arg(cardsPerVerb);
        }
        return 0;
    }

    MainWindow window;
    window.show();
    return app.exec();
}
