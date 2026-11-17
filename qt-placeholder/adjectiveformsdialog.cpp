#include "adjectiveformsdialog.h"

#include <QDialogButtonBox>
#include <QFont>
#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>

AdjectiveFormsDialog::AdjectiveFormsDialog(const AdjectiveCatalog &catalog,
                                           const QString &adjective, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("%1 — adjective endings").arg(adjective));
    resize(840, 540);
    setMinimumSize(620, 380);
    auto *layout = new QVBoxLayout(this);
    auto *heading = new QLabel(QStringLiteral("%1 — %2")
                                   .arg(adjective, catalog.english.value(adjective)));
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    heading->setFont(headingFont);
    layout->addWidget(heading);
    auto *note = new QLabel("Forms used before a noun. In the mixed group, ein has no plural; "
                            "kein and possessive determiners follow the same adjective endings.");
    note->setWordWrap(true);
    layout->addWidget(note);

    auto *forms = new QTreeWidget;
    forms->setHeaderLabels({QStringLiteral("Pattern / case"), QStringLiteral("Masculine"),
                            QStringLiteral("Feminine"), QStringLiteral("Neuter"),
                            QStringLiteral("Plural")});
    forms->setAlternatingRowColors(true);
    forms->setEditTriggers(QAbstractItemView::NoEditTriggers);
    forms->setColumnWidth(0, 230);
    for (int column = 1; column < 5; ++column) forms->setColumnWidth(column, 135);
    forms->header()->setStretchLastSection(true);
    for (const QString &pattern : adjectivePatterns()) {
        auto *group = new QTreeWidgetItem(forms);
        group->setText(0, adjectivePatternName(pattern));
        QFont groupFont = group->font(0);
        groupFont.setBold(true);
        group->setFont(0, groupFont);
        for (const QString &caseKey : catalog.nouns.cases) {
            auto *row = new QTreeWidgetItem(group);
            row->setText(0, caseName(caseKey));
            const QStringList referents = adjectiveReferents();
            for (int column = 0; column < referents.size(); ++column)
                row->setText(column + 1,
                             catalog.form(adjective, pattern, caseKey, referents.at(column)));
        }
    }
    forms->expandAll();
    layout->addWidget(forms);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}
