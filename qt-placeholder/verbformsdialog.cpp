#include "verbformsdialog.h"

#include <QDialogButtonBox>
#include <QFont>
#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>

VerbFormsDialog::VerbFormsDialog(const VerbCatalog &catalog, const QString &verb,
                                 QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("%1 — verb forms").arg(verb));
    resize(720, 620);
    setMinimumSize(480, 380);

    auto *layout = new QVBoxLayout(this);
    auto *heading = new QLabel(QStringLiteral("%1 — %2").arg(verb, catalog.english.value(verb)));
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    headingFont.setPointSize(headingFont.pointSize() + 2);
    heading->setFont(headingFont);
    layout->addWidget(heading);

    auto *forms = new QTreeWidget;
    forms->setHeaderLabels({QStringLiteral("Tense / subject"), QStringLiteral("Correct form")});
    forms->setAlternatingRowColors(true);
    forms->setUniformRowHeights(true);
    forms->setEditTriggers(QAbstractItemView::NoEditTriggers);
    forms->setColumnWidth(0, 190);
    forms->header()->setStretchLastSection(true);

    for (const QString &tense : catalog.tenses) {
        auto *tenseItem = new QTreeWidgetItem(forms);
        tenseItem->setText(0, spanishTenseName(tense));
        QFont tenseFont = tenseItem->font(0);
        tenseFont.setBold(true);
        tenseItem->setFont(0, tenseFont);

        for (const QString &subject : catalog.subjects) {
            auto *subjectItem = new QTreeWidgetItem(tenseItem);
            subjectItem->setText(0, spanishSubjectName(subject));
            subjectItem->setText(1, catalog.forms.value(
                QStringLiteral("Verbs-%1-%2").arg(tense, subject)).value(verb));
        }
    }
    forms->expandAll();
    layout->addWidget(forms);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}
