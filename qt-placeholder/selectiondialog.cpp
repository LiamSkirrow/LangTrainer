#include "selectiondialog.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSet>
#include <QSettings>
#include <QSignalBlocker>
#include <QVBoxLayout>

SelectionDialog::SelectionDialog(const SelectionDialogConfig &config, QWidget *parent)
    : QDialog(parent), itemName_(config.itemName), settingsKey_(config.settingsKey)
{
    setWindowTitle(config.title);
    resize(440, 520);
    setMinimumSize(350, 380);

    auto *layout = new QVBoxLayout(this);
    auto *instruction = new QLabel(config.instruction);
    instruction->setWordWrap(true);
    layout->addWidget(instruction);

    auto *search = new QLineEdit;
    search->setPlaceholderText(config.searchPlaceholder);
    search->setClearButtonEnabled(true);
    layout->addWidget(search);

    auto *selectionRow = new QHBoxLayout;
    auto *selectAll = new QPushButton("Select all");
    auto *clearAll = new QPushButton("Clear all");
    selectAll->setToolTip(QStringLiteral("Selects all %1, including those hidden by the search.")
                              .arg(itemName_));
    clearAll->setToolTip(QStringLiteral("Clears all %1, including those hidden by the search.")
                             .arg(itemName_));
    selectionRow->addWidget(selectAll);
    selectionRow->addWidget(clearAll);
    selectionRow->addStretch();
    layout->addLayout(selectionRow);

    itemList_ = new QListWidget;
    itemList_->setAlternatingRowColors(true);
    layout->addWidget(itemList_);

    QSettings settings;
    const bool hasSavedSelection = settings.contains(settingsKey_);
    QSet<QString> saved;
    for (const QString &value : settings.value(settingsKey_).toStringList())
        saved.insert(value);
    for (const SelectionOption &option : config.options) {
        auto *item = new QListWidgetItem(option.label, itemList_);
        item->setData(Qt::UserRole, option.value);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(!hasSavedSelection || saved.contains(option.value)
                                ? Qt::Checked : Qt::Unchecked);
    }

    countLabel_ = new QLabel;
    layout->addWidget(countLabel_);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel);
    continueButton_ = buttons->addButton(config.continueLabel, QDialogButtonBox::AcceptRole);
    if (config.showDetails) {
        auto *detailsButton = buttons->addButton(config.detailsLabel, QDialogButtonBox::ActionRole);
        detailsButton->setEnabled(false);
        detailsButton->setToolTip("Highlight one row to view its details.");
        connect(itemList_, &QListWidget::currentItemChanged, this,
                [detailsButton](QListWidgetItem *current) {
                    detailsButton->setEnabled(current != nullptr);
                });
        connect(detailsButton, &QPushButton::clicked, this,
                [this, showDetails = config.showDetails] {
                    if (const auto *item = itemList_->currentItem())
                        showDetails(item->data(Qt::UserRole).toString(), this);
                });
    }
    layout->addWidget(buttons);

    connect(search, &QLineEdit::textChanged, this, [this](const QString &query) {
        for (int i = 0; i < itemList_->count(); ++i) {
            auto *item = itemList_->item(i);
            item->setHidden(!item->text().contains(query, Qt::CaseInsensitive));
        }
        if (auto *current = itemList_->currentItem(); current && current->isHidden())
            itemList_->setCurrentItem(nullptr);
    });
    connect(selectAll, &QPushButton::clicked, this, [this] {
        const QSignalBlocker blocker(itemList_);
        for (int i = 0; i < itemList_->count(); ++i)
            itemList_->item(i)->setCheckState(Qt::Checked);
        updateCount();
    });
    connect(clearAll, &QPushButton::clicked, this, [this] {
        const QSignalBlocker blocker(itemList_);
        for (int i = 0; i < itemList_->count(); ++i)
            itemList_->item(i)->setCheckState(Qt::Unchecked);
        updateCount();
    });
    connect(itemList_, &QListWidget::itemChanged, this, [this] { updateCount(); });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(continueButton_, &QPushButton::clicked, this, [this] {
        QSettings settings;
        settings.setValue(settingsKey_, selectedValues());
        accept();
    });
    updateCount();
    search->setFocus();
}

QStringList SelectionDialog::selectedValues() const
{
    QStringList result;
    for (int i = 0; i < itemList_->count(); ++i) {
        const auto *item = itemList_->item(i);
        if (item->checkState() == Qt::Checked)
            result.append(item->data(Qt::UserRole).toString());
    }
    return result;
}

void SelectionDialog::updateCount()
{
    const int count = selectedValues().size();
    countLabel_->setText(QStringLiteral("%1 of %2 %3 selected")
                             .arg(count).arg(itemList_->count()).arg(itemName_));
    continueButton_->setEnabled(count > 0);
}
