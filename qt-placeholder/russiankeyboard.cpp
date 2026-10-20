#include "russiankeyboard.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

QWidget *createRussianKeyboard(QLineEdit *answerEdit, QWidget *parent)
{
    auto *keyboard = new QWidget(parent);
    auto *layout = new QVBoxLayout(keyboard);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    const QStringList rows = {QStringLiteral("йцукенгшщзхъ"),
                              QStringLiteral("фывапролджэ"),
                              QStringLiteral("ячсмитьбюё")};
    for (int row = 0; row < rows.size(); ++row) {
        auto *keyLayout = new QHBoxLayout;
        keyLayout->setSpacing(3);
        if (row == 0)
            keyLayout->addWidget(new QLabel("Insert:"));
        else
            keyLayout->addSpacing(44);
        for (const QChar character : rows.at(row)) {
            const QString letter(character);
            auto *button = new QToolButton(keyboard);
            button->setText(letter);
            button->setToolTip(QStringLiteral("Insert %1").arg(letter));
            button->setFixedWidth(29);
            keyLayout->addWidget(button);
            QObject::connect(button, &QToolButton::clicked, answerEdit, [answerEdit, letter] {
                answerEdit->insert(letter);
                answerEdit->setFocus();
            });
        }
        if (row == rows.size() - 1) {
            auto *spaceButton = new QToolButton(keyboard);
            spaceButton->setText("Space");
            spaceButton->setToolTip("Insert a space between words");
            keyLayout->addWidget(spaceButton);
            QObject::connect(spaceButton, &QToolButton::clicked, answerEdit, [answerEdit] {
                answerEdit->insert(QStringLiteral(" "));
                answerEdit->setFocus();
            });
        }
        keyLayout->addStretch();
        layout->addLayout(keyLayout);
    }
    return keyboard;
}
