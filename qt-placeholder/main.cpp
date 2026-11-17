#include "verbdata.h"
#include "noundata.h"
#include "nounformsdialog.h"
#include "nounexercisedialog.h"
#include "verbexercisedialog.h"
#include "verbformsdialog.h"
#include "selectiondialog.h"
#include "pronoundata.h"
#include "pronounformsdialog.h"
#include "pronounexercisedialog.h"

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
        resize(780, 620);
        setMinimumSize(650, 540);

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
        languagePicker->addItem("German (Deutsch)", QStringLiteral("German"));
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
        auto *germanGroup = createGermanPracticeGroup();
        spanishGroup->hide();
        russianGroup->hide();
        germanGroup->hide();
        layout->addWidget(spanishGroup);
        layout->addWidget(russianGroup);
        layout->addWidget(germanGroup);
        layout->addStretch();
        setCentralWidget(central);

        connect(languagePicker, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [languagePicker, spanishGroup, russianGroup, germanGroup, selectionHint](int) {
                    const QString language = languagePicker->currentData().toString();
                    spanishGroup->setVisible(language == QLatin1String("Spanish"));
                    russianGroup->setVisible(language == QLatin1String("Russian"));
                    germanGroup->setVisible(language == QLatin1String("German"));
                    selectionHint->setText(language.isEmpty()
                        ? "Select a language to see its practice modes."
                        : QStringLiteral("%1 is selected. Choose a practice mode below.")
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
        auto *nounButton = addPracticeRow(grid, 1, "Noun declension",
            "Practise singular noun forms across Russian cases.", "Choose nouns...", true);
        connect(nounButton, &QPushButton::clicked, this, [this] { startNounPractice(QStringLiteral("Russian")); });
        auto *prepositionButton = addPracticeRow(grid, 2, "Prepositions",
            "Choose the noun form required by a preposition.", "Choose nouns...", true);
        connect(prepositionButton, &QPushButton::clicked, this, [this] { startPrepositionPractice(QStringLiteral("Russian")); });
        addPracticeRow(grid, 3, "Sentence practice",
            "Use words and grammar together in context.", "Practise", false);
        addPracticeRow(grid, 4, "Listening and speaking",
            "Respond to spoken prompts.", "Practise", false);
        return group;
    }

    QGroupBox *createGermanPracticeGroup()
    {
        auto *group = new QGroupBox("German practice modes");
        auto *grid = new QGridLayout(group);
        grid->setHorizontalSpacing(18);
        grid->setVerticalSpacing(13);
        grid->setColumnStretch(1, 1);

        auto *verbButton = addPracticeRow(grid, 0, "Verb conjugation",
            "Practise common and separable verbs across people and tenses.", "Choose verbs...", true);
        connect(verbButton, &QPushButton::clicked, this, [this] { startVerbPractice(QStringLiteral("German")); });
        auto *nounButton = addPracticeRow(grid, 1, "Noun declension",
            "Practise articles and noun forms in every case and number.", "Choose nouns...", true);
        connect(nounButton, &QPushButton::clicked, this, [this] { startNounPractice(QStringLiteral("German")); });
        auto *pronounButton = addPracticeRow(grid, 2, "Pronoun declension",
            "Practise personal and possessive pronouns in all four cases.", "Choose pronouns...", true);
        connect(pronounButton, &QPushButton::clicked, this, [this] { startPronounPractice(); });
        auto *prepositionButton = addPracticeRow(grid, 3, "Prepositions",
            "Choose the noun phrase required by each preposition.", "Choose nouns...", true);
        connect(prepositionButton, &QPushButton::clicked, this, [this] { startPrepositionPractice(QStringLiteral("German")); });
        addPracticeRow(grid, 4, "Sentence practice",
            "Use words and grammar together in context.", "Practise", false);
        addPracticeRow(grid, 5, "Listening and speaking",
            "Respond to spoken prompts.", "Practise", false);
        return group;
    }

    void startPronounPractice()
    {
        PronounCatalog catalog;
        QString error;
        if (!loadGermanPronounCatalog(catalog, error)) {
            QMessageBox::critical(this, "Vocabulary error", error);
            return;
        }

        SelectionDialogConfig pronounConfig;
        pronounConfig.title = "Choose German pronouns";
        pronounConfig.instruction =
            "Choose personal or possessive pronouns. Highlight one to view its forms. "
            "Possessive cards cover forms before a noun and standalone forms.";
        pronounConfig.searchPlaceholder = "Search pronouns or English meanings...";
        pronounConfig.itemName = "pronouns";
        pronounConfig.settingsKey = "German/pronounDeclension/selectedPronouns";
        pronounConfig.continueLabel = "Next: cases";
        pronounConfig.detailsLabel = "View forms...";
        pronounConfig.showDetails = [&catalog](const QString &pronoun, QWidget *parent) {
            PronounFormsDialog forms(catalog, pronoun, parent);
            forms.exec();
        };
        for (const QString &pronoun : catalog.pronouns) {
            const QString family = catalog.kind.value(pronoun) == QLatin1String("personal")
                ? QStringLiteral("Personal") : QStringLiteral("Possessive");
            pronounConfig.options.append({pronoun,
                QStringLiteral("%1 · %2").arg(family, catalog.displayName(pronoun))});
        }
        SelectionDialog pronouns(pronounConfig, this);
        if (pronouns.exec() != QDialog::Accepted)
            return;

        SelectionDialogConfig caseConfig;
        caseConfig.title = "Choose German pronoun cases";
        caseConfig.instruction =
            "Choose cases to practise. Genitive personal pronouns are rare in everyday speech.";
        caseConfig.searchPlaceholder = "Search cases...";
        caseConfig.itemName = "cases";
        caseConfig.settingsKey = "German/pronounDeclension/selectedCases";
        caseConfig.continueLabel = "Start practice";
        for (const QString &caseKey : catalog.cases)
            caseConfig.options.append({caseKey, caseName(caseKey)});
        SelectionDialog cases(caseConfig, this);
        if (cases.exec() != QDialog::Accepted)
            return;

        PronounExerciseDialog exercise(catalog, pronouns.selectedValues(),
                                       cases.selectedValues(), this);
        exercise.exec();
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

    bool chooseNouns(const NounCatalog &catalog, const QString &modeKey,
                            const QString &continueLabel, QStringList &selected)
    {
        SelectionDialogConfig config;
        config.title = QStringLiteral("Choose %1 nouns").arg(catalog.language);
        config.instruction = "Check nouns to practise. Highlight one to view its forms.";
        config.searchPlaceholder = "Search nouns or English meanings...";
        config.itemName = "nouns";
        config.settingsKey = QStringLiteral("%1/%2/selectedNouns").arg(catalog.language, modeKey);
        config.continueLabel = continueLabel;
        config.detailsLabel = "View forms...";
        config.showDetails = [&catalog](const QString &noun, QWidget *parent) {
            NounFormsDialog forms(catalog, noun, parent);
            forms.exec();
        };
        QStringList sortedNouns = catalog.nouns;
        sortedNouns.sort(Qt::CaseInsensitive);
        for (const QString &noun : sortedNouns) {
            const QString gender = catalog.gender.value(noun);
            const QString article = gender == QLatin1String("m") ? QStringLiteral("der")
                : gender == QLatin1String("f") ? QStringLiteral("die") : QStringLiteral("das");
            const QString label = catalog.language == QLatin1String("German")
                ? QStringLiteral("%1 %2 — %3").arg(article, noun, catalog.english.value(noun))
                : QStringLiteral("%1 — %2").arg(noun, catalog.english.value(noun));
            config.options.append({noun, label});
        }
        SelectionDialog nouns(config, this);
        if (nouns.exec() != QDialog::Accepted)
            return false;
        selected = nouns.selectedValues();
        return true;
    }

    void startNounPractice(const QString &language)
    {
        NounCatalog catalog;
        QString error;
        if (!loadNounCatalog(language, catalog, error)) {
            QMessageBox::critical(this, "Vocabulary error", error);
            return;
        }
        QStringList selectedNouns;
        if (!chooseNouns(catalog, QStringLiteral("nounDeclension"),
                                QStringLiteral("Next: cases"), selectedNouns))
            return;

        SelectionDialogConfig caseConfig;
        caseConfig.title = language == QLatin1String("German")
            ? QStringLiteral("Choose German cases and numbers")
            : QStringLiteral("Choose %1 cases").arg(language);
        caseConfig.instruction = language == QLatin1String("German")
            ? QStringLiteral("Choose the case and number combinations for this session.")
            : QStringLiteral("Choose the cases for this practice session.");
        caseConfig.searchPlaceholder = "Search cases...";
        caseConfig.itemName = language == QLatin1String("German")
            ? QStringLiteral("case/number combinations") : QStringLiteral("cases");
        caseConfig.settingsKey = language + QStringLiteral("/nounDeclension/selectedCases");
        caseConfig.continueLabel = "Start practice";
        for (const QString &number : catalog.numbers) {
            for (const QString &caseKey : catalog.cases) {
                const QString key = language == QLatin1String("German")
                    ? number + QLatin1Char('-') + caseKey : caseKey;
                const QString label = language == QLatin1String("German")
                    ? QStringLiteral("%1 — %2").arg(number == QLatin1String("plural")
                          ? QStringLiteral("Plural") : QStringLiteral("Singular"), caseName(caseKey))
                    : caseName(caseKey);
                caseConfig.options.append({key, label});
            }
        }
        SelectionDialog cases(caseConfig, this);
        if (cases.exec() != QDialog::Accepted)
            return;

        NounExerciseDialog exercise(catalog, selectedNouns, cases.selectedValues(),
                                    NounExerciseDialog::Mode::Declension, this);
        exercise.exec();
    }

    void startPrepositionPractice(const QString &language)
    {
        NounCatalog catalog;
        QString error;
        if (!loadNounCatalog(language, catalog, error)) {
            QMessageBox::critical(this, "Vocabulary error", error);
            return;
        }
        QStringList selectedNouns;
        if (!chooseNouns(catalog, QStringLiteral("prepositions"),
                                QStringLiteral("Next: prepositions"), selectedNouns))
            return;

        SelectionDialogConfig prepConfig;
        prepConfig.title = QStringLiteral("Choose %1 prepositions").arg(language);
        prepConfig.instruction = "Choose prepositions. The required case is revealed after each answer.";
        prepConfig.searchPlaceholder = "Search prepositions or meanings...";
        prepConfig.itemName = "prepositions";
        prepConfig.settingsKey = language + QStringLiteral("/prepositions/selectedPrepositions");
        prepConfig.continueLabel = "Start practice";
        for (const QString &preposition : catalog.prepositions)
            prepConfig.options.append({preposition, QStringLiteral("%1 — %2")
                                           .arg(catalog.prepositionSpelling.value(preposition),
                                                catalog.prepositionEnglish.value(preposition))});
        SelectionDialog prepositions(prepConfig, this);
        if (prepositions.exec() != QDialog::Accepted)
            return;

        NounExerciseDialog exercise(catalog, selectedNouns, prepositions.selectedValues(),
                                    NounExerciseDialog::Mode::Prepositions, this);
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
        for (const QString &language : {QStringLiteral("Spanish"), QStringLiteral("Russian"),
                                        QStringLiteral("German")}) {
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
        for (const QString &language : {QStringLiteral("Russian"), QStringLiteral("German")}) {
            NounCatalog nouns;
            QString error;
            if (!loadNounCatalog(language, nouns, error)) {
                qCritical().noquote() << error;
                return 1;
            }
            qInfo().noquote() << QStringLiteral("Loaded %1 %2 nouns, %3 cases, and %5 prepositions (number categories: %4).")
                                     .arg(nouns.nouns.size()).arg(language).arg(nouns.cases.size())
                                     .arg(nouns.numbers.size()).arg(nouns.prepositions.size());
        }
        PronounCatalog pronouns;
        QString pronounError;
        if (!loadGermanPronounCatalog(pronouns, pronounError)) {
            qCritical().noquote() << pronounError;
            return 1;
        }
        qInfo().noquote() << QStringLiteral("Loaded %1 German pronoun bases across %2 cases.")
                                 .arg(pronouns.pronouns.size()).arg(pronouns.cases.size());
        return 0;
    }

    MainWindow window;
    window.show();
    return app.exec();
}
