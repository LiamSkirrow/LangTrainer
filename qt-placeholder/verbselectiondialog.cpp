#include "verbselectiondialog.h"

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

namespace {
constexpr auto selectionKey = "Spanish/verbConjugation/selectedVerbs";
}

VerbSelectionDialog::VerbSelectionDialog(const VerbCatalog &catalog, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Choose Spanish verbs");
    resize(440, 520);
    setMinimumSize(350, 380);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Choose the verbs for this practice session."));

    auto *search = new QLineEdit;
    search->setPlaceholderText("Search verbs or English meanings...");
    search->setClearButtonEnabled(true);
    layout->addWidget(search);

    auto *selectionRow = new QHBoxLayout;
    auto *selectAll = new QPushButton("Select all");
    auto *clearAll = new QPushButton("Clear all");
    selectAll->setToolTip("Selects all verbs, including those hidden by the search.");
    clearAll->setToolTip("Clears all verbs, including those hidden by the search.");
    selectionRow->addWidget(selectAll);
    selectionRow->addWidget(clearAll);
    selectionRow->addStretch();
    layout->addLayout(selectionRow);

    verbList_ = new QListWidget;
    verbList_->setAlternatingRowColors(true);
    layout->addWidget(verbList_);

    QSettings settings;
    const bool hasSavedSelection = settings.contains(QLatin1String(selectionKey));
    QSet<QString> saved;
    for (const QString &verb : settings.value(QLatin1String(selectionKey)).toStringList())
        saved.insert(verb);
    QStringList verbs = catalog.verbs;
    verbs.sort(Qt::CaseInsensitive);
    for (const QString &verb : verbs) {
        auto *item = new QListWidgetItem(
            QStringLiteral("%1 — %2").arg(verb, catalog.english.value(verb)), verbList_);
        item->setData(Qt::UserRole, verb);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(!hasSavedSelection || saved.contains(verb)
                                ? Qt::Checked : Qt::Unchecked);
    }

    countLabel_ = new QLabel;
    layout->addWidget(countLabel_);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel);
    startButton_ = buttons->addButton("Start practice", QDialogButtonBox::AcceptRole);
    layout->addWidget(buttons);

    connect(search, &QLineEdit::textChanged, this, [this](const QString &query) {
        for (int i = 0; i < verbList_->count(); ++i) {
            auto *item = verbList_->item(i);
            item->setHidden(!item->text().contains(query, Qt::CaseInsensitive));
        }
    });
    connect(selectAll, &QPushButton::clicked, this, [this] {
        const QSignalBlocker blocker(verbList_);
        for (int i = 0; i < verbList_->count(); ++i)
            verbList_->item(i)->setCheckState(Qt::Checked);
        updateCount();
    });
    connect(clearAll, &QPushButton::clicked, this, [this] {
        const QSignalBlocker blocker(verbList_);
        for (int i = 0; i < verbList_->count(); ++i)
            verbList_->item(i)->setCheckState(Qt::Unchecked);
        updateCount();
    });
    connect(verbList_, &QListWidget::itemChanged, this, [this] { updateCount(); });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(startButton_, &QPushButton::clicked, this, [this] {
        QSettings settings;
        settings.setValue(QLatin1String(selectionKey), selectedVerbs());
        accept();
    });
    updateCount();
    search->setFocus();
}

QStringList VerbSelectionDialog::selectedVerbs() const
{
    QStringList result;
    for (int i = 0; i < verbList_->count(); ++i) {
        const auto *item = verbList_->item(i);
        if (item->checkState() == Qt::Checked)
            result.append(item->data(Qt::UserRole).toString());
    }
    return result;
}

void VerbSelectionDialog::updateCount()
{
    const int count = selectedVerbs().size();
    countLabel_->setText(QStringLiteral("%1 of %2 verbs selected")
                             .arg(count).arg(verbList_->count()));
    startButton_->setEnabled(count > 0);
}
