#include "sentenceconstructiondialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFont>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QStringList>
#include <QVBoxLayout>

namespace {

QGroupBox *grammarGroup(const QString &title, const QStringList &topics)
{
    auto *group = new QGroupBox(title);
    auto *layout = new QVBoxLayout(group);
    for (const QString &topic : topics)
        layout->addWidget(new QCheckBox(topic));
    layout->addStretch();
    return group;
}

} // namespace

SentenceConstructionDialog::SentenceConstructionDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("German Sentence Construction");
    resize(620, 540);
    setMinimumSize(440, 380);

    auto *layout = new QVBoxLayout(this);
    auto *status = new QLabel("TODO: Work in progress");
    QFont statusFont = status->font();
    statusFont.setBold(true);
    status->setFont(statusFont);
    layout->addWidget(status);

    auto *instruction = new QLabel(
        "Choose grammar points for future English-to-German translation cards. "
        "These checkboxes are a preview; sentence cards and saved selections are not available yet.");
    instruction->setWordWrap(true);
    layout->addWidget(instruction);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto *content = new QWidget;
    auto *grid = new QGridLayout(content);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    grid->addWidget(grammarGroup("Verb forms", {
        "Present", "Simple past", "Perfect", "Future", "Separable verbs"}), 0, 0);
    grid->addWidget(grammarGroup("Noun cases", {
        "Nominative", "Accusative", "Dative", "Genitive"}), 0, 1);
    grid->addWidget(grammarGroup("Pronouns", {
        "Personal pronouns", "Possessive pronouns"}), 1, 0);
    grid->addWidget(grammarGroup("Adjective endings", {
        "After definite articles", "After ein/kein/possessives", "Without an article"}), 1, 1);
    grid->addWidget(grammarGroup("Prepositions", {
        "Fixed-case prepositions", "Two-way prepositions"}), 2, 0);
    grid->addWidget(grammarGroup("Sentence structure", {
        "Main-clause word order", "Questions", "Negation"}), 2, 1);
    grid->addWidget(grammarGroup("Subordinate clauses", {
        "Conjunction clauses (verb-final)", "Indirect questions (verb-final)"}), 3, 0);
    grid->addWidget(grammarGroup("Relative clauses", {
        "Nominative relative pronoun", "Accusative relative pronoun",
        "Dative relative pronoun", "Genitive relative pronoun"}), 3, 1);
    scroll->setWidget(content);
    layout->addWidget(scroll);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    auto *start = buttons->addButton("Start practice", QDialogButtonBox::AcceptRole);
    start->setEnabled(false);
    start->setToolTip("Sentence cards are not available yet.");
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}
