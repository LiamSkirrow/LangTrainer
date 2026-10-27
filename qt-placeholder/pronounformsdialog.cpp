#include "pronounformsdialog.h"
#include "noundata.h"

#include <QDialogButtonBox>
#include <QFont>
#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>

PronounFormsDialog::PronounFormsDialog(const PronounCatalog &catalog,
                                       const QString &pronoun, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("%1 — pronoun forms").arg(catalog.lemma.value(pronoun)));
    resize(690, 600);
    setMinimumSize(470, 380);
    auto *layout = new QVBoxLayout(this);
    auto *heading = new QLabel(catalog.displayName(pronoun));
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    heading->setFont(headingFont);
    heading->setWordWrap(true);
    layout->addWidget(heading);
    if (catalog.kind.value(pronoun) == QLatin1String("personal")) {
        auto *note = new QLabel("Genitive personal forms are rare in everyday German.");
        note->setWordWrap(true);
        layout->addWidget(note);
    }

    auto *forms = new QTreeWidget;
    forms->setHeaderLabels({QStringLiteral("Use / referent / case"), QStringLiteral("Correct form")});
    forms->setAlternatingRowColors(true);
    forms->setUniformRowHeights(true);
    forms->setEditTriggers(QAbstractItemView::NoEditTriggers);
    forms->setColumnWidth(0, 295);
    forms->header()->setStretchLastSection(true);

    if (catalog.kind.value(pronoun) == QLatin1String("personal")) {
        for (const QString &caseKey : catalog.cases) {
            auto *item = new QTreeWidgetItem(forms);
            item->setText(0, caseName(caseKey));
            item->setText(1, catalog.answers(pronoun, caseKey).join(QStringLiteral(" / ")));
        }
    } else {
        for (const QString &use : {QStringLiteral("before"), QStringLiteral("alone")}) {
            auto *useItem = new QTreeWidgetItem(forms);
            useItem->setText(0, pronounUseName(use));
            QFont useFont = useItem->font(0);
            useFont.setBold(true);
            useItem->setFont(0, useFont);
            for (const QString &referent : {QStringLiteral("m"), QStringLiteral("f"),
                                            QStringLiteral("n"), QStringLiteral("pl")}) {
                auto *referentItem = new QTreeWidgetItem(useItem);
                referentItem->setText(0, pronounReferentName(referent));
                for (const QString &caseKey : catalog.cases) {
                    auto *caseItem = new QTreeWidgetItem(referentItem);
                    caseItem->setText(0, caseName(caseKey));
                    caseItem->setText(1, catalog.answers(pronoun, caseKey, use, referent)
                                              .join(QStringLiteral(" / ")));
                }
            }
        }
    }
    forms->expandAll();
    layout->addWidget(forms);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}
