#pragma once

#include <QHash>
#include <QString>
#include <QStringList>

struct NounCatalog {
    QString language;
    QStringList nouns;
    QStringList cases;
    QStringList numbers;
    QHash<QString, QString> english;
    QHash<QString, QString> gender;
    QHash<QString, QHash<QString, QString>> forms;
    QStringList prepositions;
    QHash<QString, QString> prepositionSpelling;
    QHash<QString, QString> prepositionEnglish;
    QHash<QString, QString> prepositionCase;

    QString form(const QString &noun, const QString &caseKey,
                 const QString &number = QStringLiteral("singular")) const;
};

bool loadNounCatalog(const QString &language, NounCatalog &catalog, QString &error);
bool loadRussianNounCatalog(NounCatalog &catalog, QString &error);
QString caseName(const QString &caseKey);
