#include "nounformsdialog.h"

#include <QDialogButtonBox>
#include <QFont>
#include <QHeaderView>
#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>

NounFormsDialog::NounFormsDialog(const NounCatalog &catalog, const QString &noun,
                                 QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("%1 — noun forms").arg(noun));
    resize(520, 460);
    setMinimumSize(360, 320);

    auto *layout = new QVBoxLayout(this);
    auto *heading = new QLabel(QStringLiteral("%1 — %2 (%3)")
                                   .arg(noun, catalog.english.value(noun), catalog.gender.value(noun)));
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    heading->setFont(headingFont);
    layout->addWidget(heading);
    auto *forms = new QTreeWidget;
    forms->setHeaderLabels({QStringLiteral("Number / case"), QStringLiteral("Correct form")});
    forms->setAlternatingRowColors(true);
    forms->setUniformRowHeights(true);
    forms->setEditTriggers(QAbstractItemView::NoEditTriggers);
    forms->setColumnWidth(0, 160);
    forms->header()->setStretchLastSection(true);
    for (const QString &number : catalog.numbers) {
        auto *numberItem = new QTreeWidgetItem(forms);
        numberItem->setText(0, number == QLatin1String("plural")
                                   ? QStringLiteral("Plural") : QStringLiteral("Singular"));
        for (const QString &caseKey : catalog.cases) {
            auto *item = new QTreeWidgetItem(numberItem);
            item->setText(0, caseName(caseKey));
            item->setText(1, catalog.form(noun, caseKey, number));
        }
    }
    forms->expandAll();
    layout->addWidget(forms);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}
