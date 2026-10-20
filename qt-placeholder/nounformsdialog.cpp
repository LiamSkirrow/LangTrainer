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
    resize(440, 420);
    setMinimumSize(360, 320);

    auto *layout = new QVBoxLayout(this);
    auto *heading = new QLabel(QStringLiteral("%1 — %2 (%3)")
                                   .arg(noun, catalog.english.value(noun), catalog.gender.value(noun)));
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    heading->setFont(headingFont);
    layout->addWidget(heading);
    layout->addWidget(new QLabel("Singular forms"));

    auto *forms = new QTreeWidget;
    forms->setHeaderLabels({QStringLiteral("Case"), QStringLiteral("Correct form")});
    forms->setAlternatingRowColors(true);
    forms->setUniformRowHeights(true);
    forms->setEditTriggers(QAbstractItemView::NoEditTriggers);
    forms->setColumnWidth(0, 160);
    forms->header()->setStretchLastSection(true);
    for (const QString &caseKey : catalog.cases) {
        auto *item = new QTreeWidgetItem(forms);
        item->setText(0, russianCaseName(caseKey));
        item->setText(1, catalog.form(noun, caseKey));
    }
    layout->addWidget(forms);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}
