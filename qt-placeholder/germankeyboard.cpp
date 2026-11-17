#include "germankeyboard.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QWidget>

QWidget *createGermanKeyboard(QLineEdit *answerEdit, QWidget *parent)
{
    auto *row = new QWidget(parent);
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    layout->addWidget(new QLabel("Insert:"));
    for (const QChar character : QStringLiteral("äöüÄÖÜß")) {
        const QString letter(character);
        auto *button = new QToolButton(row);
        button->setText(letter);
        button->setToolTip(QStringLiteral("Insert %1").arg(letter));
        button->setFixedWidth(34);
        layout->addWidget(button);
        QObject::connect(button, &QToolButton::clicked, row, [answerEdit, letter] {
            answerEdit->insert(letter);
            answerEdit->setFocus();
        });
    }
    layout->addStretch();
    return row;
}
