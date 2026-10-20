#pragma once

#include <QHash>
#include <QString>
#include <QStringList>

struct NounCatalog {
    QStringList nouns;
    QStringList cases;
    QHash<QString, QString> english;
    QHash<QString, QString> gender;
    QHash<QString, QHash<QString, QString>> forms;
    QStringList prepositions;
    QHash<QString, QString> prepositionRussian;
    QHash<QString, QString> prepositionEnglish;
    QHash<QString, QString> prepositionCase;

    QString form(const QString &noun, const QString &caseKey) const;
};

bool loadRussianNounCatalog(NounCatalog &catalog, QString &error);
QString russianCaseName(const QString &caseKey);
